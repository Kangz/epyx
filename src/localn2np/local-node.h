/**
 * Local Node management
 * Simulation of N2NP
 * Each "local node" only can communicate with their neighboors
 */

#ifndef EPYX_LOCAL_NODE_H
#define EPYX_LOCAL_NODE_H

#include "../core/mutex.h"
#include "../n2np/n2np-packet.h"
#include "local-relay.h"
#include <list>
#include <map>

namespace Epyx
{
    // Recv callback
    class LocalNode;

    class LocalNode
    {
    private:

        //Internal definition used for the callback
        typedef bool (ReceiveCb)(LocalNode&, const N2npPacket&, void* cbData);
        typedef struct{
            ReceiveCb* cb;
            void* arg;
        }ReceiveCbData;

        N2npNodeId id;
        LocalRelay *relay;

        // Packet queue
        std::list<N2npPacket> packetQueue;
        Mutex packetQueueMutex;

        // Running thread
        Thread runThread;

        // Callbacks for Recv
        std::map<N2npPacketType, ReceiveCbData> recvCallbacks;
        Mutex recvCallbacksMutex;

        // Disable copy
        LocalNode(const LocalNode&);
        const LocalNode& operator=(const LocalNode&);

    public:
        LocalNode();
        friend std::ostream& operator<<(std::ostream& os, const LocalNode& node);
        friend std::ostream& operator<<(std::ostream& os, const LocalNode *node);

        /**
         * Attach the node to another relay
         */
        void attach(LocalRelay *relay);

        /**
         * Send packet to another node
         */
        void send(const N2npNodeId& to, const N2npPacket& pkt);

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

        /**
         * Register a receive callback
         */
        void registerRecv(const N2npPacketType& type, ReceiveCb *cb, void* cbData);
    };
}

#endif /* EPYX_LOCAL_NODE_H */
