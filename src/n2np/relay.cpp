#include "relay.h"
#include "../net/tcpsocket.h"

namespace Epyx
{
    namespace N2NP
    {

        Relay::Relay(const Address& addr)
        :WorkerPool(1, true, "Relay@" + addr.toString()),
        addr(addr), id(addr) {
        }

        void Relay::treat(N2NP::Packet *pkt) {
            EPYX_ASSERT(pkt != NULL);
            // TODO: Send a message back to from node when an error happens

            // Send packet to another relay
            const Address toAddr = pkt->to.getRelay();
            if (toAddr != addr) {
                TCPSocket toSock(toAddr);
                if (!pkt->send(toSock)) {
                    log::error << "[Relay " << id << "] Unable to transmit packet "
                        << *pkt << log::endl;
                    return;
                }
                // OK, packet was successfully transmitted
                return;
            }

            // Packet is for one of my nodes. Let's find it in the map !
            const std::string toName = pkt->to.getName();
            NodeInfo *node = NULL;
            this->nodesMutex.lock();
            if (this->nodes.count(toName))
                node = (*(this->nodes.find(toName))).second;
            this->nodesMutex.unlock();

            if (node == NULL) {
                log::debug << "[Relay " << id << "] Destination not found: "
                    << *pkt << log::endl;
                return;
            }

            // Send packet
            EPYX_ASSERT(node->sock != NULL);
            if (!pkt->send(*(node->sock))) {
                log::debug << "[Relay " << id << "] Unable to send packet "
                    << *pkt << log::endl;
                return;
            }
        }
    }
}
