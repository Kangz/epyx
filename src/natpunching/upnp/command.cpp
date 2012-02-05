/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) Aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#include "command.h"
#include <list>


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
        }
        Command::Command(std::string addr,unsigned short port) {
            command = "";
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
                exit(-1); //Let's at least Verify We don't send incomplete data
            }
            dom_command = TiXmlDocument();
            TiXmlDeclaration *decl = new TiXmlDeclaration("1.0","","");
            dom_command.LinkEndChild(decl); //Creation de l'entete
            
            TiXmlElement * envelope = new TiXmlElement(SOAPPREFIX":Envelope");
            envelope->SetAttribute("xmlns:"SOAPPREFIX,"http://schemas.xmlsoap.org/soap/envelope");
            envelope->SetAttribute(SOAPPREFIX":encodingStyle","http://schemas.xmlsoap.org/encoding");
            dom_command.LinkEndChild(envelope);
            
            TiXmlElement * body = new TiXmlElement(SOAPPREFIX":Body");
            envelope->LinkEndChild(body);
            
            TiXmlElement * action_command = new TiXmlElement((SERVICEPREFIX":"+actiontype).c_str());
            action_command->SetAttribute("xmlns:"SERVICEPREFIX,service.c_str());
            body->LinkEndChild(action_command);
            std::list<TiXmlElement> list_args = std::list<TiXmlElement>();
            for(std::map<std::string,std::string>::iterator it=args.begin(); it!=args.end(); ++it){
                list_args.push_back(TiXmlElement(it->first.c_str()));
                list_args.back().LinkEndChild( new TiXmlText(it->second.c_str()) );
                action_command->LinkEndChild( &(list_args.back()) );
            }
            action_command->LinkEndChild(new TiXmlText("") );
            
            //XML Data Built. Now we export it into this->command.
            TiXmlPrinter printer = TiXmlPrinter();
            printer.SetIndent("    ");
            dom_command.Accept( &printer );
            command = printer.CStr();
            
            //The Content is now build. Now we need to add the headers.
            std::stringstream httpcommand;
            httpcommand << "POST " << this->path << " HTTP/1.1"                            << std::endl;
            httpcommand << "Host: " << this->address <<":"<<this->port                     << std::endl;
            httpcommand << "User-Agent: Epyx Natpunching FTW"                              << std::endl;
            httpcommand << "Content-Length: " << command.length()                          << std::endl;
            httpcommand << "Content-Type: text/xml"                                        << std::endl;
            httpcommand << "SOAPAction: \""<< this->service << "#"<< actiontype << "\""    << std::endl;
            httpcommand << "Connection: Close"                                             << std::endl;
            httpcommand << "Cache-Control: no-cache"                                       << std::endl;
            httpcommand << "Pragma: no-cache"                                              << std::endl;
            httpcommand << std::endl;
            httpcommand << command;
            command = httpcommand.str();
            
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
            default :
                return "";
            }
        }
        
        void Command::Parse(){ 
            dom_answer = TiXmlDocument();
            dom_answer.Parse((const char*) raw_answer.c_str(),0,TIXML_DEFAULT_ENCODING); 
            dom_answer.SaveFile("toto.xml");
            
            
        }

        void Command::send(){
            s = new Socket(address,port);
            s->connect();
            s->write(command);
        }
        
        void Command::Receive(){
            char data[BIG_SIZE];
            s->recv((void*)data, BIG_SIZE);
            /*for (int i = 0 ; i < bytes ; i++){
                if (data[i] < 32)
                    std::cout << (int) data[i];
                std::cout << data[i];
            }
            std::cout << std::endl;*/
            raw_answer = std::string(data); //We may want to strip header from raw_data
            
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
