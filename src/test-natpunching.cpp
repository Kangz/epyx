#include "api.h"
#include "natpunching/upnp/openconnect.h"
#include "natpunching/upnp/discovery.h"
#include "net/uri.h"

bool test_IGD() {
    // Discover IGD
    Epyx::URI uri;
    Epyx::UPNP::Discovery disco;
    if (!disco.queryAnswerIn(10000, &uri)) {
        Epyx::log::error << "UPnP discovery failed" << Epyx::log::endl;
        return false;
    }
    Epyx::log::debug << "URI : " << uri << Epyx::log::endl;

    // Found an IGD
    Epyx::UPNP::IGD igd(uri);
    if (!igd.getServices()) {
        Epyx::log::error << "Unable to get IGD services" << Epyx::log::endl;
        return false;
    }

    Epyx::log::debug << "service list:" << Epyx::log::endl;
    std::map<std::string, std::string> services = igd.getServiceList();
    for (std::map<std::string, std::string>::iterator it = services.begin(); it != services.end(); ++it) {
        Epyx::log::debug << " * " << it->first << " = " << it->second << Epyx::log::endl;
    }

    Epyx::log::debug << "External IP addr is " << igd.getExtIPAdress() << Epyx::log::endl;
    Epyx::log::debug << "Local IP addr is " << igd.getLocalAdress() << Epyx::log::endl;

    // PortMap
    Epyx::SockAddress addr = igd.addPortMap(22, Epyx::UPNP::TCP, 1337);
    Epyx::log::debug << "External " << addr << " now maps to local port 22" << Epyx::log::endl;

    // List
    Epyx::log::info << "List of mappings:" << Epyx::log::endl;
    std::list<Epyx::UPNP::portMap> mappings = igd.getListPortMap();
    for (auto it = mappings.begin(); it != mappings.end(); ++it) {
        Epyx::log::info << (it->enabled ? "*" : "O") << " "
            << it->protocol << " " << it->nat_port << " to " << it->destination
            << " " << it->description << Epyx::log::endl;
    }

    Epyx::log::info << "Does it work? " << Epyx::log::endl;
    std::string blah;
    std::cin >> blah;
    if (!igd.delPortMap(addr, Epyx::UPNP::TCP)) {
        Epyx::log::debug << "Unable to delete portmap" << Epyx::log::endl;
        return false;
    }
    Epyx::log::debug << "Portmap deleted" << Epyx::log::endl;
    return true;
}

int main() {
    Epyx::API epyx;
    try {
        //Epyx::UPNP::Natpunch natpunch;
        //natpunch.openMapPort(22, 1337);
        test_IGD();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
