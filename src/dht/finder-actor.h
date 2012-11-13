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
#ifndef EPYX_DHT_FINDER_ACTOR_H
#define EPYX_DHT_FINDER_ACTOR_H

#include "../core/common.h"
#include "../core/actor.h"
#include "internal-node.h"
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
#include "value.h"
#include "process-actors.h"

namespace Epyx
{
namespace DHT
{

    class InternalNode;

    typedef std::vector<std::pair<Distance, Peer::SPtr>> ClosestQueue;

    /**
     * @class FindCallback
     * @brief the user-defined callback for te find operation
     */
    class FindCallback {
    public:
        /**
         * @brief called when the search is finished
         * @param result the closest nodes found
         */
        virtual void onFound(ClosestQueue& result) = 0;

        /**
         * @brief called when an error occurs
         */
        virtual void onError();

        /**
         * @brief FindCallback destructor
         */
        virtual ~FindCallback();
    };

    //A comparator used in the algorithm
    struct FindNearestComparator {
        bool operator()(const std::pair<Distance, Peer::SPtr> a, const std::pair<Distance, Peer::SPtr> b) const;
    };

    static FindNearestComparator heap_comp;

    /**
     * @class FinderActor
     * @brief the meat of the DHT: finds the closest nodes to an Id
     *
     * The FinderActor will first search the closest nodes the Node knows
     * and add them to the shortlist (list of the k closest nodes not contacted
     * so far), then it sends queries to all these nodes. It will enter a cycle
     * where each answer will bring new nodes in the shortlist. When the shortlist
     * is empty, we know we are close to the result.
     */
    class FinderActor: public ProcessActor {
    public:
        /**
         * @brief FinderActor constructor
         * @param n the InternalNode
         * @param idToFind
         * @param cb te user-defined callback
         */
        FinderActor(InternalNode& n, const Id& idToFind, int count, FindCallback* cb);

        /**
         * @brief start the FinderActor after it has been added in a manager
         */
        void start();

        void timeout();
    protected:
        virtual void onNewAnswer(Peer::SPtr peer, Packet::UPtr pkt);
        virtual void onAnswerTimeout(long id);

        //helper functions to preserve the heap structure and the size of the lists
        void addToShortList(Peer::SPtr p);
        void addToFoundPeers(Peer::SPtr p);
        void addToContactedPeers(Peer::SPtr p);

    private:
        void onResponse();
        void sendFindQueryTo(Peer::SPtr p);
        bool sendNewQuery();

        ClosestQueue foundPeers;
        ClosestQueue shortlist;
        std::map<Distance, Peer::SPtr> contactedPeers; //Index them by the distance which should be unique
        int countToFind;
        int pendingRequests;
        Id requestedId;
        FindCallback* callback;
    };

}
}

#endif //EPYX_DHT_FINDER_ACTOR_H
