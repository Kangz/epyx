/**
 */
#ifndef EPYX_DHT_NODE_H
#define EPYX_DHT_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "internal-node.h"
#include "kbucket.h"
#include "packet.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    class Node
    {
    public:
        Node(Id& id, const std::string& name);
        ~Node();

        void eatN2NP(const N2NP::NodeId& senderId, const N2NP::NodeId& myself, char* data, int dataSize);
        void send(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget);
    private:
        Id id;
        GTTParser gttParser;
        InternalNode n;

   };

}
}

#endif //EPYX_DHT_NODE_H
