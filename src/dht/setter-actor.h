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
#ifndef EPYX_DHT_SETTER_ACTOR_H
#define EPYX_DHT_SETTER_ACTOR_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
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
