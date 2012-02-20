/**
 * Local Relay management
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_RELAY_H
#define EPYX_LOCAL_RELAY_H

#include "../n2np/n2np-packet.h"
#include "../net/address.h"
#include "../core/mutex.h"
#include "../core/thread.h"
#include "../core/blocking-queue.h"
#include <ostream>
#include <map>

namespace Epyx
{
    class LocalNode;

    class LocalRelay : public Runnable
    {
    private:
        Address addr;
        N2npNodeId id;
        std::map<std::string, LocalNode*> nodes;
        unsigned int lastNodeId;
        Mutex nodesMutex;

        // Packet queue
        BlockingQueue<N2npPacket> packetQueue;

        // Disable copy
        LocalRelay(const LocalRelay&);
        const LocalRelay& operator=(const LocalRelay&);

    public:
        LocalRelay(Address addr_);

        friend std::ostream& operator<<(std::ostream& os, const LocalRelay& relay);
        friend std::ostream& operator<<(std::ostream& os, const LocalRelay *relay);

        /**
         * Return the new node ID
         */
        N2npNodeId attachNode(LocalNode *node);

        /**
         * Another thread post a packet
         */
        inline void post(const N2npPacket& pkt)
        {
            packetQueue.push(new N2npPacket(pkt));
        }

        /**
         * Close packet queue
         */
        inline void close()
        {
            packetQueue.close();
        }

        /**
         * Internal loop
         */
        void run();
    };
}

#endif /* EPYX_LOCAL_RELAY_H */
