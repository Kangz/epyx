
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
#include "../../net/tcpsocket.h"
#include "../../net/httpheaders.h"
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
        void IGD::parseRootDescFile(TiXmlElement * actualNode){
            if     (strcmp(actualNode->Value(),"service") == 0 ) //If true, actualNode->Value() == "service"
                this->services[actualNode->FirstChildElement("serviceType")->GetText()]=actualNode->FirstChildElement("controlURL")->GetText();
            else if(strcmp(actualNode->Value(),"serviceList") == 0)
                parseRootDescFile(actualNode->FirstChildElement());
            else if(strcmp(actualNode->Value(),"deviceList") == 0)
                parseRootDescFile(actualNode->FirstChildElement());
            else if(strcmp(actualNode->Value(),"device") == 0)
                parseRootDescFile(actualNode->FirstChildElement());

            if (actualNode->NextSiblingElement() != NULL)
                parseRootDescFile(actualNode->NextSiblingElement());
        }

        void IGD::getServices(){
            std::stringstream header;
            header << "GET " << rootDescPath << " HTTP/1.1" << endl
                   << "Host: "<< this->address << endl
                   << "Connection: Close" << endl
                   << "User-Agent: Epyx Natpunching FTW" << endl << endl;
            TCPSocket sock(address);
            sock.connect();
            sock.write(header.str());
            char data[BIG_SIZE];
            int bytes = sock.recv(data, BIG_SIZE);
            std::string reply = data;
            int packetSize = HTTPHeaders::getlength(HTTPHeaders::getHeaders(reply));
            while (bytes < packetSize) {
                memset(data,'\0',BIG_SIZE); // Needed for the working of sock.recv()
                bytes += sock.recv( (void *) data, BIG_SIZE);
                reply.append(data);
            }
            std::string rootDesc = HTTPHeaders::stripHeaders(reply);
            //We got the rootDesc data. We now need to parse it to find all the available services.
            TiXmlDocument domRootDesc;
            domRootDesc.Parse(rootDesc.c_str());
            parseRootDescFile(domRootDesc.FirstChildElement()->FirstChildElement());
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
            order.buildCommand();
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
                order.buildCommand();
                order.send();
                order.Receive();
                returnedHeader = HTTPHeaders::getHeaders(order.getAnswer());
                order.Parse();
                std::map<std::string,std::string> parsedResult = order.getResult();
                unsigned short extPort = (unsigned short) atoi(parsedResult["NewExternalPort"].c_str());
                unsigned short intPort = (unsigned short) atoi(parsedResult["NewInternalPort"].c_str());
                Epyx::Address newInternalAddress(parsedResult["NewInternalClient"].c_str(),intPort,4);
                portMap tmp={(parsedResult["NewEnabled"]!="0"),
                    newInternalAddress,
                    extPort,
                    parsedResult["NewProtocol"],
                    parsedResult["NewPortMappingDescription"]
                    };
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
                    if (memcmp(addressBuffer, this->address.ip(), 3) == 0) { // the condition is true is the first 3 bytes of addressBuffer are equal to the first 3 bytes of the ip address.
                        //This is needed to change by comparing the address with netmasks. For example, first comparing the addresses with applied netmask, then, if ok, turn into string with the inet_top method.
                        IPAddress = addressBuffer;
                        break;
                    }
                }
            }
            if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
            return IPAddress;
        }
        const Epyx::Address IGD::addPortMap(unsigned short port, protocol proto){
            return this->addPortMap(port,proto,port);
        }
        const Epyx::Address IGD::addPortMap(unsigned short loc_port, protocol proto, unsigned short ext_port){
            std::string prot = (proto == Epyx::UPNP::TCP)? "TCP" : "UDP";
            std::cout<< "Entering addPortMap("<<loc_port<<","<<prot<<","<<ext_port<<")"<<std::endl;
            std::string localIP = this->getLocalAdress();
            std::cout << "Local IP Address is " << localIP << std::endl;
            Epyx::UPNP::Command order (this->address.ip(),this->address.getPort());
            order.setOption(Epyx::UPNP::UPNP_ACTION_ADDPORTMAP);

            char loc_portChar[10],ext_portChar[10];
            sprintf(loc_portChar,"%d",loc_port);
            sprintf(ext_portChar,"%d",ext_port);

            order.addArgument("NewRemoteHost","");
            order.addArgument("NewExternalPort",ext_portChar);
            order.addArgument("NewProtocol",prot);
            order.addArgument("NewInternalPort",loc_portChar);
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
            std::cout << "Selected service is "<< WanIPConnService <<" His Control Remote path is " << WanIPConnCtl << std::endl;
            order.setService(WanIPConnService);
            order.setPath(WanIPConnCtl);
            order.buildCommand();
            std::cout << "Sending ..." << std::endl;
            order.send();
            std::cout << "Sent!" << std::endl << "Waiting for answer" << std::endl;
            order.Receive();
            std::cout << "Answer received ! : " << order.getAnswer() << std::endl;
            if (HTTPHeaders::getHeaders(order.getAnswer()).find("500 Internal") != std::string::npos){
                std::cerr << "IGD : Couldn't add port map" << std::endl;
                throw FailException("IGD","Couldn't add requested Port Map");
            }
            const Epyx::Address extAddress(this->getExtIPAdress().c_str(),ext_port,4);
            return extAddress;
        }
        void IGD::delPortMap(Address addr, protocol proto){
            std::string prot = (proto == Epyx::UPNP::TCP)? "TCP" : "UDP";
            Epyx::UPNP::Command order (this->address.ip(),this->address.getPort());
            order.setOption(Epyx::UPNP::UPNP_ACTION_DELPORTMAP);

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
            order.buildCommand();
            order.send();
            order.Receive();
            if (HTTPHeaders::getHeaders(order.getAnswer()).find("500 Internal")!= std::string::npos)
                throw FailException("IGD","Couldn't delete requested Port Map");
        }
        std::map<std::string,std::string> IGD::getServiceList(){
            return this->services;
        }
    }
}
