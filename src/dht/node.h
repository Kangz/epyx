/**
 */
#ifndef EPYX_DHT_NODE_H
#define EPYX_DHT_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "kbucket.h"
#include "dht-packet.h"

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

        void send(DHTPacket& pkt, Id target, N2NP::NodeId n2npTarget);

        void sendPong(Id target, N2NP::NodeId n2npTarget);
        void sendGot(DHTPacket& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleGot(DHTPacket& pkt);
        void sendStored(DHTPacket& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleStored(DHTPacket& pkt);
        void sendFound(DHTPacket& pkt, Id target, N2NP::NodeId n2npTarget);
        void handleFound(DHTPacket& pkt);

   };

}
}

#endif //EPYX_DHT_NODE_H
