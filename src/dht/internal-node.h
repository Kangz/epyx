/**
 * @file dht/internal-node.h
 * @brief Private structure of Node
 */
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
#include "process-actors.h"
#include "target.h"

namespace Epyx
{
namespace DHT
{

    class Node;

    struct StaticActorData;
    struct ProcessActorData;

    class InternalNode
    {
    public:
        InternalNode(const Id& id, Node& parent, const std::string& name);
        ~InternalNode();

        void processPacket(Packet& pkt, Target& target);
        void send(Packet& pkt, const Target& target);

        long registerProcessActor(Actor<ProcessActorData>& actor);
        void unregisterProcessActor(long actorNumber);

        //TODO: avoid making these public
        ActorManager actors;
        ActorId<StaticActorData> pingActor;
        ActorId<StaticActorData> getActor;
        ActorId<StaticActorData> storeActor;
        ActorId<StaticActorData> findActor;

        atom::Counter processActorsCount;
        atom::Map<long, ActorId<ProcessActorData>*> processActors;

        Id id;
        Node& parent;
        KBucket kbucket;
        Storage storage;

    private:
        void sendPong(Target& target);

        void dispatchToProcessActor(Packet& pkt, Target& target);

   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
