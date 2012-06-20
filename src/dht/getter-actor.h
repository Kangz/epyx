/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
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

#endif //EPYX_DHT_SETTER_ACTOR_H
