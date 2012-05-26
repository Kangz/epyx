#ifndef EPYX_DHT_PROCESS_ACTORS_H
#define EPYX_DHT_PROCESS_ACTORS_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "target.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    struct ProcessActorData{
        Target& target;
        Packet* pkt;

        ProcessActorData(Target& target, Packet* pkt);
        ProcessActorData(Target& target, Packet& pkt);
        void freeData();
    };
}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
