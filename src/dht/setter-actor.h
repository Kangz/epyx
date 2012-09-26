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
 * @file dht/setter-actor.h
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

    /**
     * @class SetCallback
     * @brief a user-defined callback for the SET operation
     */
    class SetCallback {
    public:
        /**
         * @brief called when the value is set
         */
        virtual void onSet() = 0;

        /**
         * @brief called when it fails
         */
        virtual void onError();
        virtual ~SetCallback();
    };

   /**
     * @class SetterActor
     * @brief The logic for the set operation
     *
     * It first asks for the closest nodes to the key then
     * send a SET query to each of these nodes
     */
    class SetterActor: public Actor {
    public:
        /**
         * @brief the SetterActor constructor
         * @param n the InternalNode
         * @param key the key
         * @param value the value
         * @param cb the user-defined callback
         */
        SetterActor(InternalNode& n, const std::string& key, const std::string& value, SetCallback* cb);

        /**
         * @brief called after the actor is added to the manager
         */
        void start();

        void treat(EPYX_AQA("find success"), std::vector<Peer>* peers);
        void treat(EPYX_AQA("find failure"));
        void treat(EPYX_AQA("set success"));
        void treat(EPYX_AQA("set failure"));
    protected:
        void onSetReceive();
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

    //The FinderActor callback used by the SetterActor
    class SetterSearchCallback: public FindCallback {
    public:
        SetterSearchCallback(ActorId<SetterActor> parent);
        void onFound(ClosestQueue& result);
        void onError();

    private:
        ActorId<SetterActor> parent;
    };

}
}

#endif //EPYX_DHT_SETTER_ACTOR_H
