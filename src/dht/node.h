/**
 */
#ifndef EPYX_DHT_NODE_H
#define EPYX_DHT_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/node.h"
#include "internal-node.h"
#include "kbucket.h"
#include "packet.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    class Node : public N2NP::Module
    {
    public:
        Node(Id& id, const std::string& name);
        ~Node();

        void fromN2NP(N2NP::Node& myself, N2NP::NodeId senderId, const char* data, unsigned int dataSize);
        void send(N2NP::Node& n2npNode, Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
    private:
        Id id;
        GTTParser gttParser;
        InternalNode n;

   };

}
}

#endif //EPYX_DHT_NODE_H
