#include "igd.h"
#include "command.h"
#include "rootdesc.h"
#include "../../net/tcpsocket.h"
#include "../../core/common.h"
#include <sstream>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#define SOCKET_ERROR (-1)

#define EPYX_MSG "Epyx Going Through your Nat with UPnP IGD Protocol"

namespace Epyx
{
    namespace UPNP
    {

        IGD::IGD(const URI& rootDescURI)
        :rootDescURI(rootDescURI), address(rootDescURI.getAddress()), services() {
        }

        bool IGD::getServices() {
            RootDesc netRootDesc(rootDescURI);
            if (!netRootDesc.queryAnswerIn(10000, &services)) {
                log::error << "Unable to load UPnP services" << log::endl;
                return false;
            }
            return true;
        }

        std::string IGD::getExtIPAdress() {
            std::string WanIPConnService, WanIPConnCtl;
            for (std::map<std::string, std::string>::iterator it = services.begin(); it != services.end(); ++it) {
                if (it->first.find("WANIPConn")) {
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            Command order(address, WanIPConnService, WanIPConnCtl);
            order.setAction(UPNP::UPNP_ACTION_GET_EXT_IP);
            std::unique_ptr<CommandResult> res = order.queryAnswer(10000);
            if (!res) {
                log::error << "UPnP: Unable to get external IP address" << log::endl;
                return "";
            }
            std::string ip = res->vars["NewExternalIPAddress"];
            return ip;
        }

        std::list<portMap> IGD::getListPortMap() {
            std::list<portMap> portMapList;
            std::string WanIPConnService, WanIPConnCtl;
            for (auto it = services.begin(); it != services.end(); ++it) {
                if (it->first.find("WANIPConn")) {
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            int httpStatus = 0;
            do {
                Command order(address, WanIPConnService, WanIPConnCtl);
                order.setAction(Epyx::UPNP::UPNP_ACTION_GET_EXT_IP); // FIXME: Is it a bug ?
                std::unique_ptr<CommandResult> res = order.queryAnswer(10000);
                if (!res) {
                    log::error << "UPnP: Unable to get portmap list" << log::endl;
                    break;
                }
                unsigned short extPort = static_cast<unsigned short> (String::toInt(res->vars["NewExternalPort"]));
                unsigned short intPort = static_cast<unsigned short> (String::toInt(res->vars["NewInternalPort"]));
                SockAddress newInternalAddress(res->vars["NewInternalClient"], intPort, 4);
                portMap tmp = {!!strcmp(res->vars["NewEnabled"].c_str(), "0"),
                    newInternalAddress,
                    extPort,
                    res->vars["NewProtocol"],
                    res->vars["NewPortMappingDescription"]};
                portMapList.push_back(tmp);
                httpStatus = res->http_status;
            } while (httpStatus != 500);
            return portMapList;
        }

        std::string IGD::getLocalAdress() {
            std::string IPAddress;
            struct ifaddrs *ifAddrStruct = NULL;
            struct ifaddrs *ifa = NULL;
            void *tmpAddrPtr = NULL;

            getifaddrs(&ifAddrStruct);

            for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa ->ifa_addr->sa_family == AF_INET) { // check it is IP4
                    // is a valid IP4 Address
                    tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
                    char addressBuffer[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                    if (memcmp(addressBuffer, this->address.getIpStr().c_str(), 3) == 0) { // the condition is true is the first 3 bytes of addressBuffer are equal to the first 3 bytes of the ip address.
                        //This is needed to change by comparing the address with netmasks. For example, first comparing the addresses with applied netmask, then, if ok, turn into string with the inet_top method.
                        IPAddress = addressBuffer;
                        break;
                    }
                }
            }
            if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);
            return IPAddress;
        }

        const SockAddress IGD::addPortMap(unsigned short port, protocol proto) {
            return this->addPortMap(port, proto, port);
        }

        const SockAddress IGD::addPortMap(unsigned short loc_port, protocol proto, unsigned short ext_port) {
            std::string prot = (proto == Epyx::UPNP::TCP) ? "TCP" : "UDP";
            log::debug << "Entering addPortMap(" << loc_port << "," << prot << "," << ext_port << ")" << log::endl;
            std::string localIP = this->getLocalAdress();
            log::debug << "Local IP Address is " << localIP << log::endl;

            std::string WanIPConnService, WanIPConnCtl;
            for (std::map<std::string, std::string>::iterator it = services.begin(); it != services.end(); ++it) {
                if (it->first.find("WANIPConn")) {
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            log::debug << "Selected service is " << WanIPConnService
                << " His Control Remote path is " << WanIPConnCtl << log::endl;
            Command order(address, WanIPConnService, WanIPConnCtl);
            order.setAction(Epyx::UPNP::UPNP_ACTION_ADDPORTMAP);

            std::string loc_portStr = String::fromInt(loc_port);
            std::string ext_portStr = String::fromInt(ext_port);

            order.addArgument("NewRemoteHost", "");
            order.addArgument("NewExternalPort", ext_portStr);
            order.addArgument("NewProtocol", prot);
            order.addArgument("NewInternalPort", loc_portStr);
            order.addArgument("NewInternalClient", localIP);
            order.addArgument("NewEnabled", "1");
            order.addArgument("NewPortMappingDescription", EPYX_MSG);
            order.addArgument("NewLeaseDuration", "0");
            std::unique_ptr<CommandResult> res = order.queryAnswer(30000);
            if (!res) {
                log::error << "IGD: Unable to add a port map" << log::endl;
                return SockAddress();
            }
            if (res->http_status == 500) {
                log::error << "IGD: Couldn't add requested port map" << log::endl;
                return SockAddress();
            }
            return SockAddress(this->getExtIPAdress(), ext_port, 4);
        }

        bool IGD::delPortMap(const SockAddress& addr, protocol proto) {
            std::string WanIPConnService, WanIPConnCtl;
            for (auto it = services.begin(); it != services.end(); ++it) {
                if (it->first.find("WANIPConn")) {
                    WanIPConnService = it->first;
                    WanIPConnCtl = it->second;
                }
            }
            Command order(address, WanIPConnService, WanIPConnCtl);
            order.setAction(Epyx::UPNP::UPNP_ACTION_DELPORTMAP);

            std::string prot = (proto == Epyx::UPNP::TCP) ? "TCP" : "UDP";

            std::string portString = String::fromInt(addr.getPort());
            order.addArgument("NewRemoteHost", "");
            order.addArgument("NewExternalPort", portString);
            order.addArgument("NewProtocol", prot);

            std::unique_ptr<CommandResult> res = order.queryAnswer(30000);
            if (!res) {
                log::error << "IGD: Unable to delete a port map" << log::endl;
                return false;
            }
            if (res->http_status == 500) {
                log::error << "IGD: Couldn't delete requested Port Map" << log::endl;
                return false;
            }
            return true;
        }

        std::map<std::string, std::string> IGD::getServiceList() {
            return services;
        }
    }
}
