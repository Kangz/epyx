#ifndef EPYX_DHT_INTERNAL_NODE_H
#define EPYX_DHT_INTERNAL_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
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

        void processPacket(N2NP::Node& myN2NP, const N2NP::NodeId& sender, Packet& pkt);

    private:
        Id id;
        Node& parent;
        KBucket kbucket;

        void sendPong(N2NP::Node& myN2NP, const Id& target, const N2NP::NodeId& n2npTarget);
        void sendGot(N2NP::Node& myN2NP, Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleGot(N2NP::Node& myN2NP, Packet& pkt);
        void sendStored(N2NP::Node& myN2NP, Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleStored(N2NP::Node& myN2NP, Packet& pkt);
        void sendFound(N2NP::Node& myN2NP, Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
        void handleFound(N2NP::Node& myN2NP, Packet& pkt);

   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
