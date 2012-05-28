#ifndef EPYX_DHT_GETTER_ACTOR_H
#define EPYX_DHT_GETTER_ACTOR_H

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

    struct GetterActorData {
        std::vector<Peer>* peersToAsk;
        bool found;
        bool answered;
        std::string result;
        GetterActorData();
        GetterActorData(std::vector<Peer>* peers);
        GetterActorData(const std::string& result);
        ~GetterActorData();
    };

    class GetCallback {
        public:
            virtual void onGot(const std::string& result) = 0;
            virtual void onError();
            virtual ~GetCallback();
    };

    class GetterSearchCallback: public FindCallback {
        public:
            GetterSearchCallback(ActorId<GetterActorData> parent);
            void onFound(ClosestQueue& result);
            void onError();

        private:
            ActorId<GetterActorData> parent;
    };

    class GetterActor: public Actor<GetterActorData> {
        public:
            GetterActor(InternalNode& n, const std::string& key, GetCallback* cb);
            void start();

        protected:
            void treat(GetterActorData& msg);
            void timeout();

        private:
            void ask(Peer& p);

            InternalNode& n;
            GetCallback* callback;
            std::string key;
            int pendingRequests;
            bool found;
    };

}
}

#endif //EPYX_DHT_FINDER_ACTOR_H
