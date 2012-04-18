/**
 * @file local-node.h
 * @brief Local Node management
 *
 * Simulation of N2NP
 * Each "local node" only can communicate with their neighboors
 */

#ifndef EPYX_LOCAL_NODE_H
#define EPYX_LOCAL_NODE_H

#include "../core/mutex.h"
#include "../n2np/packet.h"
#include "../core/worker-pool.h"
#include "local-relay.h"
#include <map>

namespace Epyx
{
    // For the recv callback
    class LocalNode;

    /**
     * @class LocalNode
     * @brief Implement local N2NP node simulation with a worker pool
     */
    class LocalNode : public WorkerPool<N2NP::Packet>
    {
    private:
        //Internal definition used for the callback
        typedef bool (ReceiveCb) (LocalNode&, const N2NP::Packet&, void* cbData);
    public:
        /**
         * @brief Constructor
         * @param name Name of the WorkerPool thread
         */
        LocalNode(const std::string& name);

        /**
         * @brief Display a node in an output stream
         * @param os Output stream
         * @param node
         */
        friend std::ostream& operator<<(std::ostream& os, const LocalNode& node);

        /**
         * @brief Attach the node to another relay
         */
        void attach(LocalRelay *relay);

        /**
         * @brief Send packet to another node
         */
        void send(const N2NP::NodeId& to, const N2NP::Packet& pkt);

        /**
         * @brief Register a receive callback
         */
        void registerRecv(const N2NP::PacketType& type, ReceiveCb *cb, void* cbData);

    protected:
        /**
         * @brief Treat a N2NP packet
         * @param pkt the packet to be processed
         */
        void treat(N2NP::Packet *pkt);

    private:

        typedef struct
        {
            ReceiveCb* cb;
            void* arg;
        } ReceiveCbData;

        N2NP::NodeId id;
        LocalRelay *relay;

        // Callbacks for Recv
        std::map<N2NP::PacketType, ReceiveCbData> recvCallbacks;
        Mutex recvCallbacksMutex;

        // Disable copy
        LocalNode(const LocalNode&);
        const LocalNode& operator=(const LocalNode&);

    };
}

#endif /* EPYX_LOCAL_NODE_H */
