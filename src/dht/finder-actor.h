#ifndef EPYX_DHT_FINDER_ACTOR_H
#define EPYX_DHT_FINDER_ACTOR_H

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

    struct FinderActorData {
        Target& target;
        std::vector<Peer>* answeredPeers;
        bool answered;
        FinderActorData(Target& t, std::vector<Peer>* peers, bool ans);
        ~FinderActorData();
    };

    class FindCallback {
        public:
            virtual void onFound() = 0;
            virtual void onError();
    };

    class FinderActor: public Actor<FinderActorData> {
        public:
            FinderActor(InternalNode& n, Id& idToFind, FindCallback* cb);

        protected:
            void treat(FinderActorData& msg);
            void timeout();

        private:
            void sendFindQueryTo(Target& t);

            InternalNode& n;
            Id requestedId;
            FindCallback* callback;
    };

}
}

#endif //EPYX_DHT_FINDER_ACTOR_H
