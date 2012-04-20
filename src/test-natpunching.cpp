#include "core/common.h"
#include "natpunching/upnp/openconnect.h"
/*
#include <iostream>
#include <sstream>
#include <map>
#include "core/common.h"
#include "net/socket.h"
#include "net/address.h"
#include "natpunching/upnp/command.h"
#include "natpunching/upnp/openconnect.h"
#include "natpunching/upnp/igd.h"
 */
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#define SOCKET_ERROR (-1)
 */
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

    // PortMap
    Epyx::Address addr = igd.addPortMap(22, Epyx::UPNP::TCP, 1337);
    Epyx::log::debug << "External " << addr << " now maps to local port 22" << Epyx::log::endl;
    Epyx::log::debug << "Does it work? " << Epyx::log::endl;
    std::string blah;
    std::cin >> blah;
    if (!igd.delPortMap(addr, Epyx::UPNP::TCP)) {
        Epyx::log::debug << "Unable to delete portmap" << Epyx::log::endl;
        return false;
    }
    Epyx::log::debug << "Portmap deleted" << Epyx::log::endl;
    return true;
}

int main(int argc, char* argv[]) {
    //std::string address, service, path;
    //unsigned short port;
    Epyx::Thread::init();
    Epyx::log::init(Epyx::log::CONSOLE, "");
    Epyx::Socket::init();

    /*
    std::cout << "Enter Address to connect with : " ;
    std::cin >> address;
    std::cout << "Enter Port : ";
    std::cin >> port;
    std::cout << "Enter Path to rootDesc.xml : " ;
    std::cin >> path;

    std::string blah;

    std::cout << "The command is being created ..." << std::endl;
    Epyx::UPNP::IGD igd;
    igd.setAddress(address, port);
    igd.setRootDescPath(path);
    igd.getServices();
    std::map<std::string,std::string> services = igd.getServiceList();
    for (std::map<std::string,std::string>::iterator it=services.begin(); it != services.end(); ++it){
        std::cout << it->first << " : " << it->second << std::endl;
    }
    std::cout << "IGD Configured. Adding port map" << std::endl;
    try{
        Epyx::Address addr = igd.addPortMap(22,Epyx::UPNP::TCP,1337);
        std::cout << "Port map added" << std::endl;
        std::cout << "Does it work? ";
        std::cin >> blah;
        std::cout << "Deleting Port Map" << std::endl;
        igd.delPortMap(addr,Epyx::UPNP::TCP);
        std::cout << "Port Map deleted" << std::endl;
    }
    catch(Epyx::FailException e){
        std::cerr << e << std::endl;
    }
     */
    try {
        //Epyx::UPNP::Natpunch natpunch;
        //natpunch.openMapPort(22, 1337);
        test_IGD();
    } catch (Epyx::FailException e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    Epyx::log::flushAndQuit();
    return 0;

}
