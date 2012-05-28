#ifndef EPYX_DHT_SETTER_ACTOR_H
#define EPYX_DHT_SETTER_ACTOR_H

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

    struct SetterActorData {
        std::vector<Peer>* peersToAsk;
        bool found;
        bool succeded;
        SetterActorData();
        SetterActorData(bool success);
        SetterActorData(std::vector<Peer>* peers);
        ~SetterActorData();
    };

    class SetCallback {
        public:
            virtual void onSet() = 0;
            virtual void onError();
            virtual ~SetCallback();
    };

    class SetterSearchCallback: public FindCallback {
        public:
            SetterSearchCallback(ActorId<SetterActorData> parent);
            void onFound(ClosestQueue& result);
            void onError();

        private:
            ActorId<SetterActorData> parent;
    };

    class SetterActor: public Actor<SetterActorData> {
        public:
            SetterActor(InternalNode& n, const std::string& keyi, const std::string& value, SetCallback* cb);
            void start();

        protected:
            void treat(SetterActorData& msg);
            void timeout();

        private:
            void ask(Peer& p);

            InternalNode& n;
            SetCallback* callback;
            std::string key;
            std::string value;
            int pendingRequests;
            int nErrors;
            bool found;
    };

}
}

#endif //EPYX_DHT_SETTER_ACTOR_H
