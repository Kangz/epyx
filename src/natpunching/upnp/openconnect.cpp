#include "openconnect.h"
#include "discovery.h"
#include "../../net/udpsocket.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

namespace Epyx
{
    namespace UPNP
    {

        Natpunch::Natpunch() {
            igd = Discovery::discoverIGD(10000);
        }

        const SockAddress Natpunch::openMapPort(unsigned short localPort,
            unsigned short remotePort) {
            if (!igd) {
                log::debug << "UPnP-IGD: openMapPort disabled as no IGD was found" << log::endl;
                return SockAddress();
            }

            //If remotePort is not set, we try to find an available one.
            if (remotePort == 0) {
                remotePort = igd->pickRandomFreePort(Epyx::UPNP::TCP);
            }

            SockAddress addr = igd->addPortMap(localPort, Epyx::UPNP::TCP, remotePort);
            log::debug << "UPnP-IGD: Added portmap : " << localPort << " -TCP- " << addr
                << log::endl;
            return addr;
        }
        
        void Natpunch::delMapPort(const SockAddress& addr, protocol proto) {
            if (!igd) {
                log::debug << "UPnP-IGD: delMapPort disabled as no IGD was found" << log::endl;
                return;
            }
            log::debug << "UPnP-IGD: Delete portmap to " << addr << log::endl;
            igd->delPortMap(addr, proto);
        }
    }
}
