/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * epyx
 * Copyright (C) Aniem 2012 <jeremy.buet@polytechnique.org>
 *
 */

#ifndef EPYX_COMMAND_H_
#define EPYX_COMMAND_H_

#include <string>
#include <map>
#include "../../../include/tinyxml/tinyxml.h"

namespace Epyx{
namespace UPNP{
typedef struct UPNP_Arg{std::string name; std::string value; }UPNP_Arg;
typedef enum UPNP_Action_type{
    UPNP_ACTION_CONNTYPE, 
    UPNP_ACTION_GET_EXT_IP, 
    UPNP_ACTION_ADDPORTMAP, 
    UPNP_ACTION_DELPORTMAP, 
    UPNP_ACTION_NUMENTRIES_PORTMAP, 
    UPNP_ACTION_SPECIFICPORTMAP, 
    UPNP_ACTION_GET_GEN_PORTMAP, 
    UPNP_ACTION_LIST_PORTMAP
}UPNP_Action;
    class Command
    {
    public:
            Command();
            Command(std::string addr, short port);
            Command(std::string filename);
            
            static UPNP_Arg buildArg(std::string name, std::string value);
            
            void setAddress(std::string addr);
            std::string getAddress();
            void setPort(short port);
            short getPort();
            
            void setOption(UPNP_Action_type type);
            void setOption(UPNP_Action_type type, std::map<std::string,std::string> args);
            void setArguments(std::map<std::string,std::string> args);
            void addArgument(std::string name, std::string value);
            
            void buildCommand();
            
            void Parse();
            void send();            
             
    protected:

    private:
            std::string command;
            std::string raw_answer;
            TiXmlDocument dom_command;
            TiXmlDocument dom_answer;
            
            UPNP_Action type;
            std::map<std::string,std::string> args;
            std::map<std::string,std::string> answers;
            
            std::string action;
            std::string service;
            
            std::string address;
            short port;
            
            bool needArgs;
            std::string findAction();
    };
}
}
#endif // EPYX_COMMAND_H_
