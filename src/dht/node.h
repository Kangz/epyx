/**
 */
#ifndef EPYX_DHT_NODE_H
#define EPYX_DHT_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "kbucket.h"
#include "packet.h"

namespace Epyx
{
namespace DHT
{

    class Node
    {
    public:
        Node();
        ~Node();

        void eatN2NP(N2NP::NodeId senderId, N2NP::NodeId myself, char* data, int dataSize);
    private:
        Id id;
        KBucket* kbucket;
        GTTParser gttParser;

        void send(Packet& pkt, Id target, N2NP::NodeId n2npTarget);

        void sendPong(Id target, N2NP::NodeId n2npTarget);
        void sendGot(Packet& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleGot(Packet& pkt);
        void sendStored(Packet& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleStored(Packet& pkt);
        void sendFound(Packet& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleFound(Packet& pkt);

   };

}
}

#endif //EPYX_DHT_NODE_H
