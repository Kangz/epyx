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
        Peer peer;
        std::vector<Peer>* answeredPeers;
        bool answered;
        FinderActorData(Peer& p, std::vector<Peer>* peers, bool ans);
        ~FinderActorData();
    };

    typedef std::vector<std::pair<Distance, Peer> > ClosestQueue;

    class FindCallback {
        public:
            virtual void onFound(ClosestQueue result) = 0;
            virtual void onError();
            virtual ~FindCallback();
    };

    struct FindNearestComparator {
        bool operator()(const std::pair<Distance, Peer> a, const std::pair<Distance, Peer> b) const;
    };

    static FindNearestComparator heap_comp;

    class FinderActor: public Actor<FinderActorData> {
        public:
            FinderActor(InternalNode& n, Id& idToFind, int count, FindCallback* cb);

        protected:
            void treat(FinderActorData& msg);
            void timeout();

            void addToShortList(Peer& p);
            void addToFoundPeers(Peer& p);
            void addToContactedPeers(Peer& p);

        private:
            void sendFindQueryTo(Peer& p);
            bool sendNewQuery();

            ClosestQueue foundPeers;
            ClosestQueue shortlist;
            std::map<Distance, Peer> contactedPeers; //Index them by the distance which should be unique
            InternalNode& n;
            int countToFind;
            int pendingRequests;
            Id requestedId;
            FindCallback* callback;
    };

}
}

#endif //EPYX_DHT_FINDER_ACTOR_H
