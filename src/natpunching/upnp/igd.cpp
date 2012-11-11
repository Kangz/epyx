#include "igd.h"
#include "command.h"
#include "rootdesc.h"
#include "../../net/netif.h"
#include "../../net/tcpsocket.h"
#include "../../core/common.h"

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

        IpAddress IGD::getExtIPAdress() {
            Command order;
            if (!this->initWanIPConnCommand(&order)) {
                return IpAddress();
            }
            order.setAction(UPNP::UPNP_ACTION_GET_EXT_IP);
            std::unique_ptr<CommandResult> res = order.queryAnswer(10000);
            if (!res) {
                log::error << "UPnP: Unable to get external IP address" << log::endl;
                return IpAddress();
            }
            IpAddress ip(res->getString("NewExternalIPAddress"));
            return ip;
        }

        std::list<portMap> IGD::getListPortMap() {
            std::list<portMap> portMapList;
            int index = 0;
            while (true) {
                Command order;
                if (!this->initWanIPConnCommand(&order)) {
                    break;
                }
                order.setAction(Epyx::UPNP::UPNP_ACTION_GET_GEN_PORTMAP);
                std::string str_index = String::fromInt(index);
                order.addArgument("NewPortMappingIndex", str_index);
                std::unique_ptr<CommandResult> res = order.queryAnswer(10000);
                if (!res) {
                    log::error << "UPnP: Unable to get portmap list" << log::endl;
                    break;
                }
                if (res->http_status != 200) {
                    break;
                }
                unsigned short intPort = res->getUShort("NewInternalPort");
                SockAddress newInternalAddress(res->getString("NewInternalClient"), intPort);
                portMap tmp = {res->getBoolean("NewEnabled"),
                    newInternalAddress,
                    res->getUShort("NewExternalPort"),
                    res->getString("NewProtocol"),
                    res->getString("NewPortMappingDescription")};
                portMapList.push_back(tmp);
                index++;
            }
            return portMapList;
        }

        IpAddress IGD::getLocalAdress() {
            // Find a network interface which has the same prefix
            std::vector<NetIf> interfaces = NetIf::getAllNet();
            for (auto iface = interfaces.begin(); iface != interfaces.end(); ++iface) {
                const IpAddress& iface_addr = iface->getAddress();
                // Force IPv4
                if (iface_addr.getVersion() != 4) continue;

                // The condition is true is the first 3 bytes of addressBuffer are equal to the first 3 bytes of the ip address.
                // This is needed to change by comparing the address with netmasks.
                // For example, first comparing the addresses with applied netmask, then, if ok, turn into string with the inet_top method.
                // TODO: use iface->netmask to compare this->address
                std::string addr = iface_addr.toString();
                if (memcmp(addr.c_str(), this->address.getIpStr().c_str(), 3) == 0) {
                    return iface_addr;
                }
            }
            return IpAddress();
        }

        const SockAddress IGD::addPortMap(unsigned short port, protocol proto) {
            return this->addPortMap(port, proto, port);
        }

        const SockAddress IGD::addPortMap(unsigned short loc_port, protocol proto, unsigned short ext_port) {
            std::string prot = (proto == Epyx::UPNP::TCP) ? "TCP" : "UDP";
            std::string loc_portStr = String::fromInt(loc_port);
            std::string ext_portStr = String::fromInt(ext_port);
            const IpAddress localIP = this->getLocalAdress();
            const IpAddress extIP = this->getExtIPAdress();
            log::debug << "IGD.AddPortMap(" << SockAddress(localIP, loc_port)
                << " -" << prot << "- "
                << SockAddress(extIP, ext_port) << ")" << log::endl;

            Command order;
            if (!this->initWanIPConnCommand(&order)) {
                return SockAddress();
            }
            order.setAction(Epyx::UPNP::UPNP_ACTION_ADDPORTMAP);
            order.addArgument("NewRemoteHost", "");
            order.addArgument("NewExternalPort", ext_portStr);
            order.addArgument("NewProtocol", prot);
            order.addArgument("NewInternalPort", loc_portStr);
            order.addArgument("NewInternalClient", localIP.toString());
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
            return SockAddress(extIP, ext_port);
        }

        bool IGD::delPortMap(const SockAddress& addr, protocol proto) {
            Command order;
            if (!this->initWanIPConnCommand(&order)) {
                return false;
            }
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

        bool IGD::initWanIPConnCommand(Command *command) const {
            EPYX_ASSERT(command != NULL);
            for (auto it = services.begin(); it != services.end(); ++it) {
                // Looking for urn:schemas-upnp-org:service:WANIPConnection
                if (it->first.find("WANIPConn")) {
                    // First is service. Second is a relative URI to the service
                    command->setRemote(address, it->first, it->second);
                    return true;
                }
            }
            log::error << "UPnP: Unable to get WAN IP Service" << log::endl;
            return false;
        }
    }
}
