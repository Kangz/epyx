/**
 * @file dht/node.h
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
#include "target.h"
#include "finder-actor.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;


    class Node : public N2NP::Module
    {
    public:
        Node(Id& id, N2NP::Node& n2npSelf, const std::string& name);
        ~Node();

        void fromN2NP(N2NP::Node& myself, N2NP::NodeId senderId, const char* data, unsigned int dataSize);
        void send(Packet& pkt, const Target& target);

        //API
        void sendPing(Target& t);
        void findClosest(FindCallback* cb, int count, Id& idToFind);

    private:
        Id id;
        GTTParser gttParser;
        InternalNode n;
    };

}
}

#endif //EPYX_DHT_NODE_H
