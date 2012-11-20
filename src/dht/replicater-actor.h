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
/*
 * @file dht/replicaterer-actor.h
 */
#ifndef EPYX_DHT_REPLICATER_ACTOR_H
#define EPYX_DHT_REPLICATER_ACTOR_H

#include "../core/common.h"
#include "../core/actor.h"
#include "value.h"
#include "setter-actor.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;
    class Storage;

    /**
     * @class ReplicaterActor
     * @brief Handles the replication of keys
     *
     * Periodially sends a set query
     */
    class ReplicaterActor: public Actor{
    public:
        ReplicaterActor(InternalNode& n);

        void treat(EPYX_AQA("new key"), std::string key);

        void timeout(EPYX_AQA("replicate"), std::string key);

    private:
        InternalNode& n;
        Storage& storage;
    };

    class ReplicaterSetCallback: public SetCallback {
    public:
        ReplicaterSetCallback(InternalNode& n, ActorId<ReplicaterActor> replicater, std::string key);
        virtual void onSet();
        virtual void onFoundPeers(std::shared_ptr<std::vector<Peer::SPtr>> peers);

    private:
        ActorId<ReplicaterActor> replicater;
        InternalNode& n;
        bool isHandlingKey;
        std::string key;
    };

}
}

#endif //EPYX_DHT_REPLICATER_ACTOR_H
