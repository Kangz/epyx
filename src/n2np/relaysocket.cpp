#include "relaysocket.h"
#include "relay.h"

namespace Epyx
{
    namespace N2NP
    {

        RelaySocket::RelaySocket(Socket *sock, Relay *relay)
        :NetSelectSocket(sock), relay(relay) {
        }

        RelaySocket::~RelaySocket() {
            // Dettach node
            EPYX_ASSERT(relay != NULL);
            relay->detachNode(nodeid);
            relay = NULL;
        }

        void RelaySocket::eat(const char *data, long size) {
            EPYX_ASSERT(relay != NULL);

            gttparser.eat(data, size);
            GTTPacket *gttpkt = NULL;
            // Loop over each packet
            while ((gttpkt = gttparser.getPacket()) != NULL) {
                // Build a N2NP packet and post it to the Relay
                Packet *n2nppkt = new Packet(*gttpkt);
                //There is a special packet that we must treat here
                if(n2nppkt->method == "ID" && n2nppkt->to == relay->getId())
                    relay->attachNode(&this->socket());
                else
                    relay->post(n2nppkt);
                delete gttpkt;
            }
            std::string error;
            if (gttparser.getError(error)) {
                log::error << "N2NP Relay: GTT Parser error for node " << nodeid
                    << ". Resetting parser." << log::endl;
                gttparser.reset();
            }
        }
    }
}
