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
 * @file dht/process-actor.h
 * @brief define process actors.
 */
#ifndef EPYX_DHT_PROCESS_ACTOR_H
#define EPYX_DHT_PROCESS_ACTOR_H

#include "../core/common.h"
#include "../core/actor.h"
#include "../n2np/nodeid.h"
#include "peer.h"
#include "packet.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    /**
     * @class ProcessActor
     * @brief the base class of every process Actor
     *
     * A process actor sends messages with a specific connectionId
     * and waits for the answer or timeouts.
     */
    class ProcessActor: public Actor {
    public:
        ProcessActor(InternalNode& n);
        virtual ~ProcessActor();

        //Fired by internal node when there is a new packet for this actor
        void treat(EPYX_AQA("process receive"), Peer::SPtr peer, Packet::SPtr pkt);
        //Fired by the actor manager when a query timed out
        void timeout(EPYX_AQA("process timeout"), long pNumber);

    protected:
        //To be used when we want to send a message, adds the connectionId
        long sendQuery(Peer::SPtr peer, Packet& pkt, Timeout timeout);
        //Callback for when we receive a message
        virtual void onNewAnswer(Peer::SPtr peer, Packet::SPtr pkt) = 0;
        //Callback for when a query times out
        virtual void onAnswerTimeout(long id);

        InternalNode& n;

    private:
        std::map<long, bool> queries;
    };

}
}

#endif //EPYX_DHT_STATIC_ACTOR_H
