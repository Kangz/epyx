/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) Aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#include "command.h"
#include "../../core/exception.h"
#include <list>
#include "../../net/httpheaders.h"
#include "../../net/tcpsocket.h"


#include <iostream>
#include <sstream>

#define SOAPPREFIX "s"
#define SERVICEPREFIX "u"
#define BIG_SIZE 4096
namespace Epyx{
    namespace UPNP{
        Command::Command(){
            command = "";
            answers = std::map<std::string,std::string>();
            address = "";
            port=-1;
            needArgs=false;
            args=std::map<std::string,std::string>();
            endl = "\r\n";
        }
        Command::Command(std::string addr,unsigned short port) {
            command = "";
            endl= "\r\n";
            answers = std::map<std::string,std::string>();
            needArgs=false;
            args=std::map<std::string,std::string>();
            this->setAddress(addr);
            this->setPort(port);
        }
        Command::Command(std::string filename){
            TiXmlDocument config(filename.c_str());
        }
        Command::~Command(){
            s->close();
            delete s;
            s=NULL;

        }

        void Command::setAddress(std::string addr){
            this->address=addr;
        }
        void Command::setPort(unsigned short port){
            this->port = port;
        }

        unsigned short Command::getPort(){
            return this->port;
        }
        std::string Command::getAddress(){
            return this->address;
        }

        void Command::setPath(std::string path){
            this->path = path;
        }
        std::string Command::getPath(){
            return this->path;
        }

        UPNP_Arg Command::buildArg(std::string name, std::string value){
            UPNP_Arg arg;
            arg.name=name;
            arg.value=value;
            return arg;
        }

        void Command::setOption(UPNP_Action_type type){
            this->type = type;
        }

        void Command::setArguments(std::map<std::string,std::string> args){
            this->args = args;
        }
        void Command::addArgument(std::string name, std::string value){
            args[name]=value;
        }

        void Command::setOption(UPNP_Action_type type, std::map<std::string,std::string> args){
            setOption(type);
            setArguments(args);
        }

        void Command::buildCommand(){ //TODO Manage Errors a bit better than this
            std::string actiontype=findAction();
            if (needArgs && args.size() == 0){
                FailException("buildCommand","You must specify arguments for this command");
            }
            dom_command = TiXmlDocument();
            TiXmlDeclaration *decl = new TiXmlDeclaration("1.0","","");
            dom_command.LinkEndChild(decl); //Creation de l'entete

            TiXmlElement * envelope = new TiXmlElement(SOAPPREFIX":Envelope");
            envelope->SetAttribute("xmlns:"SOAPPREFIX,"http://schemas.xmlsoap.org/soap/envelope/");
            envelope->SetAttribute(SOAPPREFIX":encodingStyle","http://schemas.xmlsoap.org/soap/encoding/");
            dom_command.LinkEndChild(envelope);

            TiXmlElement * body = new TiXmlElement(SOAPPREFIX":Body");
            envelope->LinkEndChild(body);

            TiXmlElement * action_command = new TiXmlElement((SERVICEPREFIX":"+actiontype).c_str());
            action_command->SetAttribute("xmlns:"SERVICEPREFIX,service.c_str());
            body->LinkEndChild(action_command);
            std::list<TiXmlElement *> list_args;
            for(std::map<std::string,std::string>::iterator it=args.begin(); it!=args.end(); ++it){
                list_args.push_back(new TiXmlElement(it->first.c_str()));
                list_args.back()->LinkEndChild( new TiXmlText(it->second.c_str()) );
                action_command->LinkEndChild( list_args.back() );
            }
            action_command->LinkEndChild(new TiXmlText("") );

            //XML Data Built. Now we export it into this->command.
            TiXmlPrinter printer = TiXmlPrinter();
            printer.SetIndent("    ");
            printer.SetLineBreak("\r\n");
            dom_command.Accept( &printer );
            command = printer.CStr();


            //The Content is now build. Now we need to add the headers.
            std::stringstream httpcommand;
            httpcommand << "POST " << this->path << " HTTP/1.1"                             << endl;
            httpcommand << "Host: " << this->address <<":"<<this->port                      << endl;
            httpcommand << "User-Agent: Epyx Natpunching FTW"                               << endl;
            httpcommand << "Content-Length: " << command.length()                           << endl;
            httpcommand << "Content-Type: text/xml"                                         << endl;
            httpcommand << "SOAPAction: \""<< this->service << "#"<< actiontype << "\""     << endl;
            httpcommand << "Connection: Close"                                              << endl;
            httpcommand << "Cache-Control: no-cache"                                        << endl;
            httpcommand << "Pragma: no-cache"                                               << endl;
            httpcommand << ""                                                               << endl;
            httpcommand << command                                                          << endl;
            command = httpcommand.str();


/*           //Let's free everything
            delete[] action_command;
            action_command = NULL;
            delete[] body;
            body = NULL;
            delete[] envelope;
            envelope = NULL;
            delete[] decl;
            decl = NULL;*/
        }

        std::string Command::findAction(){
            switch(this->type){
            case UPNP_ACTION_CONNTYPE :
                return "GetConnectionTypeInfo";
            case UPNP_ACTION_GET_EXT_IP :
                return "GetExternalIPAddress";
            case UPNP_ACTION_ADDPORTMAP :
                needArgs=true;
                return "AddPortMapping";
            case UPNP_ACTION_DELPORTMAP :
                needArgs=true;
                return "DeletePortMapping";
            case UPNP_ACTION_NUMENTRIES_PORTMAP :
                return "GetPortMappingNumberOfEntries";
            case UPNP_ACTION_SPECIFICPORTMAP :
                needArgs=true;
                return "GetSpecificPortMappingEntry";
            case UPNP_ACTION_LIST_PORTMAP :
                needArgs=true;
                return "GetListOfPortMappings";
            case UPNP_ACTION_GET_GEN_PORTMAP :
                needArgs=true;
                return "GetGenericPortMappingEntry";
            case UPNP_ACTION_GET_STATINFO:
                needArgs=false;
                return "GetStatusInfo";
            default :
                return "";
            }
        }

        void Command::Parse(){
            std::string answer;
            answer = Epyx::HTTPHeaders::stripHeaders(this->raw_answer);
            dom_answer = TiXmlDocument();
            dom_answer.Parse((const char*) answer.c_str(),0,TIXML_DEFAULT_ENCODING);
            dom_answer.SaveFile("toto.xml");
            TiXmlPrinter printer = TiXmlPrinter();
            printer.SetIndent("    ");
            printer.SetLineBreak("\r\n");
            dom_answer.Accept( &printer );
            std::cout << printer.CStr() << std::endl;
            TiXmlNode * balise = dom_answer.RootElement();
            while (!(strstr(balise->Value(),"Body")))
                balise =  balise->FirstChild(); //Each Parent of <s:Body> has only an only Child
            balise = balise->FirstChild();
            for( TiXmlNode * child = balise->FirstChild(); child; child = child->NextSibling() ){
                answers[child->Value()]=child->FirstChild()->Value(); //Normally, child is of form <Name>Value</Name>
            }


        }

        std::map<std::string,std::string> Command::getResult(){
            return this->answers;
        }

        void Command::send(){
            std::cout << "Received send Command" << std::endl;
            TCPSocket s(Address(address.c_str(), (unsigned short)port));
            std::cout << "The socket has been initialized with address " << s.getAddress() << std::endl;
            std::cout << "Connecting ..." << std::endl;
            s.connect();
            std::cout << "Writing command "<<std::endl<< this->command <<std::endl;
            s.write(this->command);
        }

        void Command::Receive(){
            char data[BIG_SIZE];
            int bytes = s->recv((void*)data, BIG_SIZE);
            /*for (int i = 0 ; i < bytes ; i++){
                if (data[i] < 32)
                    std::cout << (int) data[i];
                std::cout << data[i];
            }
            std::cout << std::endl;*/
            raw_answer = std::string(data);
            //We better assure we got all the data. Maybe thatpart will need to be implemented in a different class/function
            int length = HTTPHeaders::getlength(HTTPHeaders::getHeaders(this->raw_answer));
            while (bytes < length){
                bytes += s->recv((void *) data, BIG_SIZE);
                raw_answer += data;
            }

        }

        std::string Command::getAnswer(){
            return this->raw_answer;
        }
        std::string Command::getService(){
            return this->service;
        }
        void Command::setService(std::string service){
            this->service = service;
        }
        std::string Command::getOrder(){
            return this->command;
        }

    }
}
