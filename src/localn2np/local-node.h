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
#include "../core/thread.h"
#include "local-relay.h"
#include <list>
#include <map>

namespace Epyx
{
    // Recv callback
    class LocalNode;

    class LocalNode : public Thread
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
        LocalNode(const std::string& threadname, int threadid);
        friend std::ostream& operator<<(std::ostream& os, const LocalNode& node);
        friend std::ostream& operator<<(std::ostream& os, const LocalNode *node);

        /**
         * @brief Attach the node to another relay
         */
        void attach(LocalRelay *relay);

        /**
         * @brief Send packet to another node
         */
        void send(const N2npNodeId& to, const N2npPacket& pkt);

        /**
         * @brief Register a receive callback
         */
        void registerRecv(const N2npPacketType& type, ReceiveCb *cb, void* cbData);

        /**
         * @brief Another thread post a packet
         */
        inline void post(const N2npPacket& pkt)
        {
            packetQueue.push(new N2npPacket(pkt));
        }

        /**
         * @brief Close packet queue
         */
        inline void close()
        {
            packetQueue.close();
        }

    protected:
        /**
         * @brief Internal loop
         */
        void run();
    };
}

#endif /* EPYX_LOCAL_NODE_H */
