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
 * @file dht/internal-node.h
 * @brief Private structure of Node
 */
#ifndef EPYX_DHT_INTERNAL_NODE_H
#define EPYX_DHT_INTERNAL_NODE_H

#include "../core/common.h"
#include "../parser/gttpacket.h"
#include "../parser/gttparser.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
#include "kbucket.h"
#include "packet.h"
#include "storage.h"
#include "static-actors.h"
#include "process-actors.h"
#include "finder-actor.h"
#include "getter-actor.h"
#include "setter-actor.h"

namespace Epyx
{
namespace DHT
{

    class Node;

    struct StaticActorData;
    struct ProcessActorData;

    class InternalNode
    {
    public:
        InternalNode(const Id& id, N2NP::Node& n2npSelf, Node& parent, const std::string& name);
        ~InternalNode();

        void processPacket(Packet& pkt, Peer& sender);
        void findClosest(FindCallback* cb, int count, Id& idToFind);
        void getValue(GetCallback* cb, const std::string& key);
        void setValue(SetCallback* cb, const std::string& key, const std::string& value);

        void send(Packet& pkt, const Peer& dest);

        long registerProcessActor(Actor<ProcessActorData>& actor, int timeout = 0);
        void unregisterProcessActor(long actorNumber);

        void sendPing(Peer& p);
        Peer getConnectionInfo();

        //TODO: avoid making these public
        ActorManager actors;
        ActorId<StaticActorData> pingActor;
        ActorId<StaticActorData> getActor;
        ActorId<StaticActorData> storeActor;
        ActorId<StaticActorData> findActor;

        atom::Counter processActorsCount;
        atom::Map<long, ActorId<ProcessActorData>*> processActors;

        Id id;
        N2NP::Node& myN2np;
        Node& parent;
        KBucket kbucket;
        Storage storage;

    private:
        void sendPong(Peer& target);

        void dispatchToProcessActor(Packet& pkt, Peer& target);

   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
