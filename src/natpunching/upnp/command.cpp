/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) Aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#include "command.h"
#include <list>
#include "../../core/socket.h"

#define SOAPPREFIX s
#define SERVICEPREFIX u
#define BIG_SIZE 4096
namespace Epyx{
Command::Command(){
    command="";
    answer="";
    address="";
    port=0;
    needArgs=false;
    args=std::map();
}
Command::Command(std::string addr, short port){
    this->Command();
    this->setAddress(addr);
    this->setPort(port);
}
Command::Command(std::string filename){
    TiXmlDocument config(filename.c_str());
}

void Command::setAddress(std::string addr){
    this->address=addr;
}
void Command::setPort(short port){
    this->port = port;
}

short Command::getPort(){
    return this->port;
}
std::string Command::getAddress(){
    return this->address;
}

UPNP_Arg Command::buildArg(std::string name, std::string value){
    UPNP_Arg arg;
    arg.name=name;
    arg.value=value;
    return arg;
}

void Command::setOption(Action_type type){
    this->type = type;
}
void Command::setArguments(std::map<std::string,std::string> args){
    this->args = args;
}
void Command::addArgument(std::string name, std::string value){
    args[name]=value;
}

void Command::setOption(Action_type type, std::map<std::string,std::string> args){
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
    
    TiXmlElement * envelope = new TiXmlElement("SOAPPREFIX:Envelope");
    envelope->SetAttribute("xmlns:SOAPPREFIX","http://schemas.xmlsoap.org/soap/envelope");
    envelope->SetAttribute("SOAPPREFIX:encodingStyle","http://schemas.xmlsoap.org/encoding");
    dom_command.LinkEndChild(envelope);
    
    TiXmlElement * body = new TiXmlElement("SOAPPREFIX:Body");
    envelope->LinkEndChild(body);
    
    TiXmlElement * action_command = new TiXmlElement("SERVICEPREFIX:"+actiontype);
    action_command->SetAttribute("xmlns:SERVICEPREFIX",service);
    body->LinkEndChild(action_command);
    std::list<TiXmlElement> list_args = new std::list();
    for(std::map<std::string,std::string>::iterator it=args.begin(); it!=args.end(); ++it){
        list_args.push_back(TiXmlElement(it->first));
        list_args.back().LinkEndChild( new TiXmlText(it->second) );
        action_command->LinkEndChild( &(list_args.back()) );
    }
    
    //XML Data Built. Now we export it into this->command.
    TiXmlPrinter printer = new TiXmlPrinter();
    printer.SetIndent("    ");
    dom_command.Accept( &printer );
    command = printer.CStr();
}

std::string Command::findAction(){
    switch(type){
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
    }
}

void Command::Parse(){ 
    dom_answer = TiXmlDocument();
    dom_answer.Parse((const char*) raw_answer.c_str(),0,TIXML_DEFAULT_ENCODING); 
    dom_answer.SaveFile("toto.xml");
    
    
}

void Command::send(){
    Socket s(address,port);
    s.write(command);
    char[BIG_SIZE] data;
    s.recvAll((void*)data, BIG_SIZE);
    raw_answer = std::string(data); //We may want to strip header from raw_data
    
}

}
