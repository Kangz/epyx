#ifndef EPYX_DHT_INTERNAL_NODE_H
#define EPYX_DHT_INTERNAL_NODE_H

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

    class Node;

    class InternalNode
    {
    public:
        InternalNode(const Id& id, Node& parent, const std::string& name);
        ~InternalNode();

        void processPacket(const N2NP::NodeId& sender, Packet& pkt);

    private:
        Id id;
        Node& parent;
        KBucket kbucket;

        void sendPong(const Id& target, const N2NP::NodeId& n2npTarget);
        void sendGot(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleGot(Packet& pkt);
        void sendStored(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleStored(Packet& pkt);
        void sendFound(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleFound(Packet& pkt);

   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
