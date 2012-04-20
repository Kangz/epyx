#include "openconnect.h"
#include "discovery.h"
#include "../../net/udpsocket.h"
#include <sstream>
#include <iostream>
#include <ctime>

#define BUF_SIZE 4096
#define TIMEOUT_SEC 5

namespace Epyx
{
    namespace UPNP
    {

        Natpunch::Natpunch()
        :success(false) {
        }

        Natpunch::~Natpunch(){
            if (igd != NULL) {
                delete igd;
                igd = NULL;
            }
        }

        const Address Natpunch::openMapPort(unsigned short localPort,
            unsigned short remotePort) {
            // Discovery UDP socket
            Discovery disco;
            if (!disco.queryAnswerIn(10000, &uri)) {
                log::error << "UPnP discovery failed" << log::endl;
                return Address();
            }

            // Now use IGD
            log::debug << "URI : " << uri << log::endl;
            igd = new IGD(uri);
            if (!igd->getServices()) {
                log::error << "Unable to get IGD services" << log::endl;
                return Address();
            }

            log::debug << "IP addr : " << igd->getExtIPAdress() << log::endl;

            Address addr = igd->addPortMap(localPort, Epyx::UPNP::TCP, remotePort);
            return addr;
        }
    }
}
