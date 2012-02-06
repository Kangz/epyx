
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#define SOCKET_ERROR (-1)

#include "igd.h"
#include "command.h"
#include "../../core/socket.h"
#include "../../core/httpheaders.h"
#include "../../../include/tinyxml/tinyxml.h"
#include <sstream>
#include <queue>



#define BIG_SIZE 4096
#define EPYX_MSG "Epyx Going Through your Nat with UPnP IGD Protocol"

namespace Epyx {
    namespace UPNP {
        std::string endl = "\r\n";
        IGD::IGD() {
            this->address = Address();
            rootDescPath = "";
        }
        IGD::IGD(Address address) {
            this->setAddress(address);
            this->setRootDescPath("");
        }
        IGD::IGD(Address address, std::string rootDescPath){
            this->setAddress(address);
            this->setRootDescPath(rootDescPath);
        }
        IGD::IGD(std::string address, unsigned short port){
            this->setAddress(address,port);
            this->setRootDescPath("");
         }
        IGD::IGD(std::string address, unsigned short port, std::string rootDescPath){
            this->setAddress(address,port);
            this->setRootDescPath(rootDescPath);
        }
        IGD::IGD(std::string rootDescPath){
            this->address = Address();
            this->setRootDescPath(rootDescPath);
        }
        
        void IGD::setAddress(Address address){
            this->address = address;
        }
        void IGD::setAddress(std::string address, unsigned short port){
            this->address = Epyx::Address(address.c_str(),port,4);
        }
        void IGD::setRootDescPath(std::string rootDescPath){
            this->rootDescPath = rootDescPath;
        }
        
        void IGD::getServices(){
            std::stringstream header;
            header << "GET /rootDesc.xml HTTP/1.1" << endl
                   << "Host: "<< this->address << endl
                   << "Connection: Close" << endl
                   << "User-Agent: Epyx Natpunching FTW" << endl << endl;
            Socket sock(address.ip(),address.getPort());
            sock.connect();
            sock.write(header.str());
            char data[BIG_SIZE];
            sock.recv(data, BIG_SIZE);
            std::string rootDesc = HTTPHeaders::stripHeaders(data);
            //We got the rootDesc data. We now need to parse it to find all the available services.
            TiXmlDocument domRootDesc;
            domRootDesc.Parse(rootDesc.c_str());
            std::queue<TiXmlElement *> nodeQueue;
            TiXmlElement * actualNode;
            nodeQueue.push(domRootDesc.FirstChildElement());
            while (!nodeQueue.empty()){
                actualNode = nodeQueue.front();
                for(TiXmlElement * serviceN = actualNode->FirstChildElement("Service"); serviceN; serviceN->NextSiblingElement("Service")){
                    this->services[serviceN->FirstChildElement("serviceType")->GetText()]=serviceN->FirstChildElement("controlURL")->GetText();
                }
                for(TiXmlElement * deviceN = actualNode->FirstChildElement("device");deviceN;deviceN->NextSiblingElement("device"))
                    nodeQueue.push(deviceN);
                for(TiXmlElement * deviceLN = actualNode->FirstChildElement("deviceList");deviceLN;deviceLN->NextSiblingElement("deviceList"))
                    nodeQueue.push(deviceLN);
                for(TiXmlElement * serviceLN = actualNode->FirstChildElement("serviceList");serviceLN;serviceLN->NextSiblingElement("serviceList"))
                    nodeQueue.push(serviceLN);
                nodeQueue.pop();
            }
        }
        
        std::string IGD::getExtIPAdress(){
            Command order (address.ip(), address.getPort());
            order.setOption(Epyx::UPNP::UPNP_ACTION_GET_EXT_IP);
            std::string WanIPConnService, WanIPConnCtl;
            for(std::map<std::string,std::string>::iterator it = services.begin(); it != services.end(); ++it ){
                if (it->first.find("WANIPConn")){
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            order.setService(WanIPConnService);
            order.setPath(WanIPConnCtl);
            order.send();
            order.Receive();
            order.Parse();
            return order.getResult()["NewExternalIPAddress"];
        }
        
        std::list<portMap> IGD::getListPortMap(){
            std::list<portMap> res;
            std::string returnedHeader ="";
            int i =0;
            do{
                Command order (address.ip(), address.getPort());
                order.setOption(Epyx::UPNP::UPNP_ACTION_GET_EXT_IP);
                std::string WanIPConnService, WanIPConnCtl;
                for(std::map<std::string,std::string>::iterator it = services.begin(); it != services.end(); ++it ){
                    if (it->first.find("WANIPConn")){
                        WanIPConnService = it->first;
                        WanIPConnCtl = it->second;
                    }
                }
                order.setService(WanIPConnService);
                order.setPath(WanIPConnCtl);
                order.send();
                order.Receive();
                returnedHeader = HTTPHeaders::getHeaders(order.getAnswer());
                order.Parse();
                std::map<std::string,std::string> parsedResult = order.getResult();
                unsigned short extPort = (unsigned short) atoi(parsedResult["NewExternalPort"].c_str());
                unsigned short intPort = (unsigned short) atoi(parsedResult["NewInternalPort"].c_str());
                portMap tmp={(parsedResult["NewEnabled"]!="0"),Epyx::Address(parsedResult["NewInternalClient"].c_str(),intPort,4),extPort,parsedResult["NewProtocol"], parsedResult["NewPortMappingDescription"]};
                res.push_back(tmp);
            }while(!returnedHeader.find("500 Internal"));
            return res;
        }
        
        std::string IGD::getLocalAdress(){
            std::string IPAddress;
            struct ifaddrs * ifAddrStruct=NULL;
            struct ifaddrs * ifa=NULL;
            void * tmpAddrPtr=NULL;
            
            getifaddrs(&ifAddrStruct);
            
            for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
                    // is a valid IP4 Address
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    char addressBuffer[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                    if (memcmp(addressBuffer, this->address.ip(), 3)) {
                        IPAddress = addressBuffer;
                        break;
                    }
                } 
            }
            if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
            return IPAddress;
        }
        Epyx::Address IGD::addPortMap(unsigned short port, protocol proto){
            std::string prot = (proto = Epyx::UPNP::TCP)? "TCP" : "UDP";
            std::string localIP = this->getLocalAdress();
            Epyx::UPNP::Command order (this->address.ip(),this->address.getPort());
            order.setOption(Epyx::UPNP::UPNP_ACTION_ADDPORTMAP);
            
            char portChar[10];
            sprintf(portChar,"%d",port);
            
            order.addArgument("NewRemoteHost","");
            order.addArgument("NewExternalPort",portChar);
            order.addArgument("NewProtocol",prot);
            order.addArgument("NewInternalPort",portChar);
            order.addArgument("NewInternalClient",localIP);
            order.addArgument("NewEnabled","1");
            order.addArgument("NewPortMappingDescription",EPYX_MSG);
            order.addArgument("NewLeaseDuration","0");
            
            
            std::string WanIPConnService, WanIPConnCtl;
            for(std::map<std::string,std::string>::iterator it = services.begin(); it != services.end(); ++it ){
                if (it->first.find("WANIPConn")){
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            order.setService(WanIPConnService);
            order.setPath(WanIPConnCtl);
            order.send();
            order.Receive();
            if (HTTPHeaders::getHeaders(order.getAnswer()).find("500 Internal"))
                throw FailException("IGD","Couldn't add requested Port Map");
            return Epyx::Address(this->getExtIPAdress().c_str(),port,4);
        }
        void IGD::delPortMap(Address addr, protocol proto){
            std::string prot = (proto = Epyx::UPNP::TCP)? "TCP" : "UDP";
            Epyx::UPNP::Command order (this->address.ip(),this->address.getPort());
            order.setOption(Epyx::UPNP::UPNP_ACTION_ADDPORTMAP);
            
            char portChar[10];
            sprintf(portChar,"%d",addr.getPort());
            
            order.addArgument("NewRemoteHost","");
            order.addArgument("NewExternalPort",portChar);
            order.addArgument("NewProtocol",prot);
            
            std::string WanIPConnService, WanIPConnCtl;
            for(std::map<std::string,std::string>::iterator it = services.begin(); it != services.end(); ++it ){
                if (it->first.find("WANIPConn")){
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            order.setService(WanIPConnService);
            order.setPath(WanIPConnCtl);
            order.send();
            order.Receive();
            if (HTTPHeaders::getHeaders(order.getAnswer()).find("500 Internal"))
                throw FailException("IGD","Couldn't delete requested Port Map");
        }
    }
}
