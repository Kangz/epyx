#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include "consts.h"
#include <algorithm>

namespace Epyx
{
namespace DHT
{

    void FindCallback::onError() {}

    FindCallback::~FindCallback() {}

    //Some helper functions for heaps
    #define heap_make(list) std::make_heap((list).begin(), (list).end(), heap_comp)
    #define heap_push(list, value) (list).push_back(value); std::push_heap((list).begin(), (list).end(), heap_comp)
    #define heap_pop_no_res(list) std::pop_heap((list).begin(), (list).end(), heap_comp); (list).pop_back()
    #define heap_pop(list, variable) std::pop_heap((list).begin(), (list).end(), heap_comp); (variable) = (list).pop_back()

    FinderActor::FinderActor(InternalNode& n, const Id& idToFind, int count, FindCallback* cb)
    :n(n), countToFind(count), pendingRequests(0), requestedId(idToFind), callback(cb) {
    }

    void FinderActor::start(){
        lock();
        heap_make(shortlist);
        heap_make(foundPeers);

        //Initialize the shortlist and foundPeers
        std::vector<Peer> nearest;
        n.kbucket.findNearestNodes(requestedId, nearest, countToFind);

        std::vector<Peer>::iterator it;
        for(it = nearest.begin(); it != nearest.end(); it ++) {
            addToShortList(*it);
            addToFoundPeers(*it);
        }

        //Send queries to the nodes closest to the target
        ClosestQueue::iterator i;
        for(i = shortlist.begin(); i != shortlist.end(); i++) {
            sendFindQueryTo((*i).second);
        }
        unlock();
    }

    void FinderActor::treat(EPYX_AQA("found"), Peer target, std::vector<Peer>* peers) {
        for(auto peerToAdd = peers->begin(); peerToAdd != peers->end(); peerToAdd ++) {
            addToShortList(*peerToAdd);
            addToFoundPeers(*peerToAdd);
        }

        delete peers;

        //Acknowledge that we received a query
        onResponse();
    }

    void FinderActor::treat(EPYX_AQA("not found"), Peer target) {
        //Acknowledge that we received a query
        onResponse();
    }

    void FinderActor::addToShortList(Peer& p) {
        Distance dist(requestedId, p.id);
        //I have a problem with containers here I need a container that can be a priority_queue
        //on which I can make a search for an element.
        //I'm making a manual search instead
        ClosestQueue::iterator it;
        for(it = shortlist.begin(); it != shortlist.end(); it++) {
            if((*it).first == dist) {
                return;
            }
        }

        heap_push(shortlist, std::make_pair(dist, p));
        if((int)shortlist.size() > FIND_PARALLEL_QUERIES) {
            heap_pop_no_res(shortlist);
        }
    }

    void FinderActor::addToFoundPeers(Peer& p) {
        Distance dist(requestedId, p.id);
        ClosestQueue::iterator it;
        for(it = foundPeers.begin(); it != foundPeers.end(); it++) {
            if((*it).second.id == p.id) {
                return;
            }
        }

        heap_push(foundPeers, std::make_pair(dist, p));
        if(foundPeers.size() > (unsigned long) countToFind) {
            heap_pop_no_res(foundPeers);
        }
    }

    void FinderActor::addToContactedPeers(Peer& p) {
        Distance dist(requestedId, p.id);
        contactedPeers.insert(std::make_pair(dist, p));
    }

    void FinderActor::timeout() {
        callback->onError();
        delete callback;
        kill();
    }

    void FinderActor::onResponse() {
        pendingRequests --;

        //Try to send a query to a new node
        bool sent = sendNewQuery();
        if(!sent) {
            //If we con't and the shortlist is empty that means we finished the search
            if(pendingRequests == 0) {
                callback->onFound(foundPeers);
                delete callback;
                kill();
            }
        }
    }

    void FinderActor::sendFindQueryTo(Peer& p) {
        addToContactedPeers(p);
        pendingRequests ++;
        new SingularFindActor(n, Actor::getId(this), p, requestedId);
    }

    bool FinderActor::sendNewQuery() {
        ClosestQueue::iterator it;
        for(it = shortlist.begin(); it != shortlist.end(); it ++) {
            if(contactedPeers.find((*it).first) == contactedPeers.end()) {
                sendFindQueryTo((*it).second);
                return true;
            }
        }
        return false;
    }
}
}
