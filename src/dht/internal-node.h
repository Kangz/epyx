#ifndef EPYX_DHT_INTERNAL_NODE_H
#define EPYX_DHT_INTERNAL_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
#include "kbucket.h"
#include "packet.h"
#include "storage.h"
#include "static-actors.h"
#include "target.h"

namespace Epyx
{
namespace DHT
{

    class Node;

    struct StaticActorData;

    class InternalNode
    {
    public:
        InternalNode(const Id& id, Node& parent, const std::string& name);
        ~InternalNode();

        void processPacket(Packet& pkt, Target& target);
        void send(Packet& pkt, const Target& target);

        //TODO: avoid making these public
        ActorManager actors;
        ActorId<StaticActorData> pingActor;
        ActorId<StaticActorData> getActor;
        ActorId<StaticActorData> storeActor;
        ActorId<StaticActorData> findActor;
        Id id;
        Node& parent;
        KBucket kbucket;
        Storage storage;

    private:
        void sendPong(Target& target);
        void handleGot(Packet& pkt, Target& target);
        void handleStored(Packet& pkt, Target& target);
        void handleFound(Packet& pkt, Target& target);

   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
