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
            // Discovery UDP socket
            Discovery disco;
            if (!disco.query()) {
                log::error << "Unable to send discovery query" << log::endl;
                return Address();
            }
            URI *puri = disco.answer(10);
            if (puri == NULL) {
                log::error << "No answer was received for UPnP discovery" << log::endl;
                return Address();
            }
            uri = *puri;
            delete puri;

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
