/**
 * Local Relay management
 * Simulation of N2NP
 */

#ifndef EPYX_LOCAL_RELAY_H
#define EPYX_LOCAL_RELAY_H

#include "../n2np/n2np-packet.h"
#include "../core/address.h"
#include "../core/mutex.h"
#include "../core/thread.h"
#include <ostream>
#include <map>
#include <list>

namespace Epyx
{
    class LocalNode;

    class LocalRelay
    {
    private:
        Address addr;
        N2npNodeId id;
        std::map<std::string, LocalNode*> nodes;
        unsigned int lastNodeId;
        Mutex nodesMutex;

        // Packet queue
        std::list<N2npPacket> packetQueue;
        Mutex packetQueueMutex;

        // Running thread
        Thread runThread;

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
        void post(const N2npPacket& pkt);

        /**
         * Internal loop
         */
        void runLoop();

        /**
         * Start a thread which runs runLoop()
         */
        void run();
    };
}

#endif /* EPYX_LOCAL_RELAY_H */
