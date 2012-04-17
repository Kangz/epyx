#include "openconnect.h"
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

        const Address Natpunch::openMapPort(unsigned short localPort,
            unsigned short remotePort) {
            // Build a new NetSelect for discovery
            NetSelect discoverSelect(1, "UPNPDiscovery");
            discoverSelect.setName("Let's listen in HTTPU");

            // Discovery UDP socket
            Discovery *listener = new Discovery();
            discoverSelect.add(listener);
            discoverSelect.start();
            if (!listener->waitAnswer(10)) {
                log::error << "No answer was received for UPnP discovery" << log::endl;
                return Address();
            }

            // Get answer (note: listener is deleted when discoverSelect is)
            uri = listener->getURI();
            discoverSelect.term();

            // Now use addr and path
            log::debug << "URI : " << uri << log::endl;
            success = true;

            /*if (success) {
                igd.setAddress(this->addr);
                igd.setRootDescPath(path);
                igd.getServices();
                try {
                    addr = igd.addPortMap(localPort, Epyx::UPNP::TCP, remotePort);

                } catch (Epyx::FailException e) {
                    std::cerr << e << std::endl; //replace by Logger.
                }
            }
            return addr;
             */
            return uri.getAddress();
        }
    }
}
