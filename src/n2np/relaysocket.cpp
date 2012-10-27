#include "relaysocket.h"
#include "relay.h"

namespace Epyx
{
    namespace N2NP
    {

        RelaySocket::RelaySocket(const std::shared_ptr<Socket>& sock, const std::shared_ptr<Relay>& relay)
        :NetSelectSocket(sock), relay(relay) {
        }

        RelaySocket::~RelaySocket() {
            // Detach node
            EPYX_ASSERT(relay);
            relay->detachNode(nodeid);
        }

        void RelaySocket::eat(const byte_str& data) {
            EPYX_ASSERT(relay);

            gttparser.eat(data);
            std::unique_ptr<GTTPacket> gttpkt;
            // Loop over each packet
            while ((gttpkt = gttparser.getPacket())) {
                // Build a N2NP packet and post it to the Relay
                std::unique_ptr<Packet> n2nppkt(new Packet(*gttpkt));
                //There is a special packet that we must treat here
                if (n2nppkt->method == "ID" && n2nppkt->to == relay->getId()) {
                    nodeid = relay->attachNode(this->socket());
                    if (nodeid.empty()) {
                        log::error << "N2NP: No node ID found" << log::endl;
                    }
                } else {
                    relay->post(std::move(n2nppkt));
                }
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
