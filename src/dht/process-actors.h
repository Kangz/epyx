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
#ifndef EPYX_DHT_PROCESS_ACTORS_H
#define EPYX_DHT_PROCESS_ACTORS_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "finder-actor.h"
#include "getter-actor.h"
#include "setter-actor.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;
    struct FinderActorData;
    struct GetterActorData;
    struct SetterActorData;

    struct ProcessActorData{
        Peer& peer;
        Packet* pkt;

        ProcessActorData(Peer& peer, Packet* pkt);
        ProcessActorData(Peer& peer, Packet& pkt);
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

    class SingularSetActor: public ProcessActor {
        public:
            SingularSetActor(InternalNode& n, ActorId<SetterActorData> p, Peer& peer, const std::string& key, const std::string& value);

        protected:
            void treat(ProcessActorData& msg);
            void timeout();

            ActorId<SetterActorData> parent;
    };

}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
