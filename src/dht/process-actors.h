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
 * @file dht/process-actors.h
 * @brief define process actors that handle answers to queries this Node sent.
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

    /**
     * @class ProcessActor
     * @brief thebase class of every process Actor
     *
     * A process actor waits for a single message (or a timeout)
     * and reports back to its parent actor as it is part of a process.
     * ProcessActor::destroy should be called instead of kill()
     */
    class ProcessActor: public Actor {
    public:
        virtual void treat(Peer* peer, Packet* pkt) = 0;

    protected:
        /**
         * @brief register the process actor in the node.
         * @param n the InternalNode
         * @param timeout the process actor timeout if it has one
         */
        ProcessActor(InternalNode& n, int timeout = 0);

        /**
         * @brief destroys the actor and unregisters it from the process
         * actor list
         */
        void destroy();

        InternalNode& n;
        long processId;
    };

    /**
     * @class SingulerFindActor
     * @brief part of a FIND process
     *
     * It sends the query and reports back to its parent
     */
    class SingularFindActor: public ProcessActor {
    public:
        SingularFindActor(InternalNode& n, ActorId<FinderActor> p, Peer& peer, Id& requested);

        virtual void treat(Peer* peer, Packet* pkt);
    protected:
        void timeout();

        Peer target;
        ActorId<FinderActor> parent;
    };

    /**
     * @class SingulerGetActor
     * @brief part of a GET process
     *
     * It sends the query and reports back to its parent
     */
    class SingularGetActor: public ProcessActor {
    public:
        SingularGetActor(InternalNode& n, ActorId<GetterActor> p, Peer& peer, const std::string& key);

        virtual void treat(Peer* peer, Packet* pkt);
    protected:
        void timeout();

        ActorId<GetterActor> parent;
    };

    /**
     * @class SingulerSetActor
     * @brief part of a SET process
     *
     * It sends the query and reports back to its parent
     */
    class SingularSetActor: public ProcessActor {
    public:
        SingularSetActor(InternalNode& n, ActorId<SetterActor> p, Peer& peer, const std::string& key, const std::string& value);

        virtual void treat(Peer* peer, Packet* pkt);
    protected:
        void timeout();

        ActorId<SetterActor> parent;
    };

}
}

#endif //EPYX_DHT_STATIC_ACTORS_H
