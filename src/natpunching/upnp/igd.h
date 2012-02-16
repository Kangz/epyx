#ifndef EPYX_IGD_H
#define EPYX_IGD_H

#include <map>
#include <string>
#include <list>
#include "../../net/address.h"
#include "../../../include/tinyxml/tinyxml.h"

namespace Epyx {
    namespace UPNP {
        /*typedef struct serviceInfo{
                std::string def_path;
                std::string ctl_path;
        }serviceInfo;
        */ // I think the struct might be useless. we'll see
        typedef struct portMap{
                bool enabled;
                Epyx::Address destination;
                unsigned short nat_port;
                std::string protocol;
                std::string description;
        }portMap;
        typedef enum prot{
            TCP,
            UDP
        }protocol;

        class IGD {
            public:
                IGD();
                IGD(Epyx::Address address);
                IGD(std::string address, unsigned short port);
                IGD(std::string rootDescPath);
                IGD(Epyx::Address address, std::string rootDescPath);
                IGD(std::string address, unsigned short port, std::string rootDescPath);
                void setAddress(Epyx::Address address);
                void setAddress(std::string address, unsigned short port);
                void setRootDescPath(std::string rootDescPath);
                std::string getExtIPAdress();
                std::list<portMap> getListPortMap();

                /**
                 * Returns Address class with which we can join the user from
                 * the internet. For compatibility reasons the remote Port must
                 * be the same as the local port
                 */
                const Epyx::Address addPortMap(unsigned short port, protocol proto);
                const Epyx::Address addPortMap(unsigned short loc_port, protocol proto,unsigned short ext_port );
                void getServices();
                std::string getLocalAdress(); //Gets the local IP adress which communicates with the IGD.
                void delPortMap(Epyx::Address addr, protocol proto); //Addr must be the IP TCP/UDP coordinates for access to user machine through IGD NAT Device. (Ex the return value of addPortMap)
                std::map<std::string,std::string> getServiceList();
            private :
                std::map<std::string,std::string> services; //First : Name of service. Second : Control Path. Let's Assume all commands of a service are always supported...
                Epyx::Address address;
                std::string rootDescPath;
                void parseRootDescFile(TiXmlElement *actualNode);

        };
    }
}

#endif // EPYX_IGD_H
