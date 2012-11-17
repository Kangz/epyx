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
 * @brief Private structure of a DHT Node
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

    class PingActor;
    class StoreActor;
    class GetActor;
    class FindActor;
    class ProcessActor;

    /**
     * @class InternalNode
     *
     * @brief Responsible of the internal state of a DHT node
     *
     * This is the central structure of the dht, holding the kbucket, the storage
     * and the actor system. Actors are split into three categories : static actors,
     * process actors and others.
     *
     * Static actors are spawned once and are used to reply to incoming queries.
     * Using actors allows us to defer the operation and return immediately.
     *
     * Process actors are used to wait for an answer to a message with a specific
     * connection id. There are created when we need long processes. Then they can
     * be used to send packets with connection ids which will be given by the
     * internal node (the internal node register the process actor at the same time
     * to deliver the packets afterward). When a packet arrives or a query times
     * out, a process actor will call a callback on its children class.
     *
     * Long network processes such a FIND operation are handled by an actor that is
     * given the user's callback for this operation. Sometimes the network just
     * fails and the actor can decide to time out. This first-level actor can
     * then spawn other actors for suboperations (e.g. GET spawns a FIND).
     */
    class InternalNode
    {
    public:

        /**
         * @brief InternalNode's constructor
         * @param id this DHT node's own id
         * @param n2npSelf the N2npId of the corresponding N2NP Node
         * @param parent the corresponding DHT Node (public interface)
         * @param name the name for the children threads
         */
        InternalNode(const Id& id, N2NP::Node& n2npSelf, Node& parent, const std::string& name);

        /**
         * @brief InternalNode's destructor
         */
        ~InternalNode();

        /**
         * @brief handles a packet received through the N2NP subsystem
         * @param pkt the DHT packet received
         * @param sender the connection information for the sender
         */
        void processPacket(Packet* pkt, const Peer& sender);

        //Is proxied by Node
        void sendPing(Peer::SPtr p);
        void findClosest(FindCallback* cb, int count, const Id& idToFind);
        void getValue(GetCallback* cb, const std::string& key);
        void setValue(SetCallback* cb, const std::string& key, const std::string& value);
        Peer::SPtr getConnectionInfo() const;

        //proxy for Node
        void send(Packet& pkt, Peer::SPtr dest);

        //Manages the processActors (see InternalNode description)
        //long registerProcessActor(ProcessActor& actor, int timeout = 0);
        //void unregisterProcessActor(long actorNumber);
        long registerNewConnectionId(ActorId<ProcessActor> actor);
        void unregisterConnectionId(long id);
        //TODO: avoid making these public

        //The actor system
        ActorManager actors;

        //Management for the process actors
        std::atomic<long> processActorsCount;
        atom::Map<long, ActorId<ProcessActor>> processActors;

        //This node's identity
        Id id;
        N2NP::Node& myN2np;
        Peer::SPtr myPeer;
        Node& parent;

        //Other components
        KBucket kbucket;
        Storage storage;

        //Static actors
        ActorId<PingActor> pingActor;
        ActorId<GetActor> getActor;
        ActorId<StoreActor> storeActor;
        ActorId<FindActor> findActor;

    private:
        void sendPong(const Peer& target);

        void dispatchToProcessActor(Packet::SPtr pkt, Peer::SPtr target);
   };

}
}

#endif //EPYX_DHT_INTERNAL_NODE_H
