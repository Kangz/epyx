#ifndef EPYX_DHT_PROCESS_ACTORS_H
#define EPYX_DHT_PROCESS_ACTORS_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "target.h"
#include "finder-actor.h"
#include "getter-actor.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;
    struct FinderActorData;
    struct GetterActorData;

    struct ProcessActorData{
        Target& target;
        Packet* pkt;

        ProcessActorData(Target& target, Packet* pkt);
        ProcessActorData(Target& target, Packet& pkt);
        void freeData();
    };

    class ProcessActor: public Actor<ProcessActorData> {
        protected:
            ProcessActor(InternalNode& n, int timeout = 0);
            void destroy();

            InternalNode& n;
            long processId;
    };

    static const int SINGLE_REQUEST_TIMEOUT = 2*1000;

    class SingularFindActor: public ProcessActor {
        public:
            SingularFindActor(InternalNode& n, ActorId<FinderActorData> p, Peer& peer, Id& requested);

        protected:
            void treat(ProcessActorData& msg);
            void timeout();

            Peer target;
            ActorId<FinderActorData> parent;
    };

    class SingularGetActor: public ProcessActor {
        public:
            SingularGetActor(InternalNode& n, ActorId<GetterActorData> p, Peer& peer, const std::string& key);

        protected:
            void treat(ProcessActorData& msg);
            void timeout();

            ActorId<GetterActorData> parent;
    };

}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
