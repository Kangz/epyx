/**
 * Local Node management
 * Simulation of N2NP
 * Each "local node" only can communicate with their neighboors
 */

#ifndef EPYX_LOCAL_NODE_H
#define EPYX_LOCAL_NODE_H

#include "../core/mutex.h"
#include "../n2np/n2np-packet.h"
#include "../core/blocking-queue.h"
#include "../core/runnable.h"
#include "local-relay.h"
#include <list>
#include <map>

namespace Epyx
{
    // Recv callback
    class LocalNode;

    class LocalNode : public Runnable
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
        BlockingQueue<N2npPacket> packetQueue;

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
         * Register a receive callback
         */
        void registerRecv(const N2npPacketType& type, ReceiveCb *cb, void* cbData);

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

#endif /* EPYX_LOCAL_NODE_H */
