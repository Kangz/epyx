#ifndef EPYX_DHT_STATIC_ACTORS_H
#define EPYX_DHT_STATIC_ACTORS_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
#include "target.h"
#include "value.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    struct StaticActorData{
        Target& target;
        Packet* pkt;

        StaticActorData(Target& target, Packet* pkt);
        StaticActorData(Target& target, Packet& pkt);
        void freeData();
    };

    class PingActor: public Actor<StaticActorData> {
    public:
        PingActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    class GetActor: public Actor<StaticActorData> {
    public:
        GetActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    class StoreActor: public Actor<StaticActorData> {
    public:
        StoreActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    class FindActor: public Actor<StaticActorData> {
    public:
        FindActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };
}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
