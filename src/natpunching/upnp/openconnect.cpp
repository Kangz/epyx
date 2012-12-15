#include "openconnect.h"
#include "discovery.h"
#include "../../net/udpsocket.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

#define BUF_SIZE 4096
#define TIMEOUT_SEC 5

namespace Epyx
{
    namespace UPNP
    {

        Natpunch::Natpunch()
        :success(false) {
        }

        const SockAddress Natpunch::openMapPort(unsigned short localPort,
            unsigned short remotePort) {
            // Discovery UDP socket
            Discovery disco;
            if (!disco.queryAnswerIn(10000, &uri)) {
                log::error << "UPnP discovery failed" << log::endl;
                return SockAddress();
            }
            //If remotePort is not set, we try to find an available one.
            // Now use IGD
            log::debug << "URI : " << uri << log::endl;
            igd.reset(new IGD(uri));
            if (!igd->getServices()) {
                log::error << "Unable to get IGD services" << log::endl;
                return SockAddress();
            }

            log::debug << "IP addr : " << igd->getExtIPAdress() << log::endl;
            //If remotePort is not set, we try to find an available one.
            if (remotePort == 0) {
                remotePort = igd->pickRandomFreePort(Epyx::UPNP::TCP);
            }

            SockAddress addr = igd->addPortMap(localPort, Epyx::UPNP::TCP, remotePort);
            return addr;
        }
    }
}
