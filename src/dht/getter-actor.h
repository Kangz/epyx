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
 * @file dht/getter-actor.h
 */
#ifndef EPYX_DHT_GETTER_ACTOR_H
#define EPYX_DHT_GETTER_ACTOR_H

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
     * @struct GetterActorData
     * @brief the type of the messages received by the GetterActor
     *
     * It can be sent by the GetterSearchCallback and contain a peer list
     * or be sent by SingularGetterActor siblings
     */
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

    /**
     * @class GetCallback
     * @brief a user-defined callback for the GET operation
     */
    class GetCallback {
    public:
        /**
         * @brief called when the value is found
         * @param result the found value
         */
        virtual void onGot(const std::string& result) = 0;

        /**
         * @brief called when it fails
         */
        virtual void onError();
        virtual ~GetCallback();
    };

    //The FinderActor callback used by the GetterActor
    class GetterSearchCallback: public FindCallback {
        public:
            GetterSearchCallback(ActorId<GetterActorData> parent);
            void onFound(ClosestQueue& result);
            void onError();

        private:
            ActorId<GetterActorData> parent;
    };

    /**
     * @class GetterActor
     * @brief The logic for the get operation
     *
     * It first asks for the closest nodes to the key then
     * send a GET query to each of these nodes
     */
    class GetterActor: public Actor<GetterActorData> {
    public:
        /**
         * @brief the GetterActor constructor
         * @param n the InternalNode
         * @param key the key
         * @param cb the user-defined callback
         */
        GetterActor(InternalNode& n, const std::string& key, GetCallback* cb);

        /**
         * @brief called after the actor is added to the manager
         */
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
