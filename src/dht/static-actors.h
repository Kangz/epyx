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
/**
 * @file dht/static-actors.h
 * @brief Internal actors for DHT
 */
#ifndef EPYX_DHT_STATIC_ACTORS_H
#define EPYX_DHT_STATIC_ACTORS_H

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

    /**
     * @struct StaticActorData
     * @brief the messages received by static actors
     */
    struct StaticActorData{
        Peer& peer;
        Packet* pkt;

        StaticActorData(Peer& peer, Packet* pkt);
        StaticActorData(Peer& peer, Packet& pkt);
        void freeData();
    };

    /**
     * @class PingActor
     * @brief handles PING queries
     */
    class PingActor: public Actor {
    public:
        PingActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    /**
     * @class GetActor
     * @brief handles GET queries
     */
    class GetActor: public Actor {
    public:
        GetActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    /**
     * @class StoreActor
     * @brief handles Store queries
     */
    class StoreActor: public Actor {
    public:
        StoreActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };

    /**
     * @class FindActor
     * @brief handles FIND queries
     */
    class FindActor: public Actor {
    public:
        FindActor(InternalNode& n);
        void treat(StaticActorData& msg);

    private:
        InternalNode& n;
    };
}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
