#include "static-actors.h"
#include "node.h"
#include "internal-node.h"
#include <algorithm>

namespace Epyx
{
namespace DHT
{

    FinderActorData::FinderActorData(Peer& p, std::vector<Peer>* peers, bool ans)
    :peer(p), answeredPeers(peers), answered(ans) {
    }

    FinderActorData::~FinderActorData() {
        if(answeredPeers != NULL){
            delete answeredPeers;
        }
    }

    void FindCallback::onError() {}

    FindCallback::~FindCallback() {}

    #define FIND_REDUNDANCY 3
    #define heap_make(list) std::make_heap((list).begin(), (list).end(), heap_comp)
    #define heap_push(list, value) (list).push_back(value); std::push_heap((list).begin(), (list).end(), heap_comp)
    #define heap_pop_no_res(list) std::pop_heap((list).begin(), (list).end(), heap_comp); (list).pop_back()
    #define heap_pop(list, variable) std::pop_heap((list).begin(), (list).end(), heap_comp); (variable) = (list).pop_back()

    FinderActor::FinderActor(InternalNode& n, Id& idToFind, int count, FindCallback* cb)
    :n(n), countToFind(count), pendingRequests(0), requestedId(idToFind), callback(cb) {
        heap_make(shortlist);
        heap_make(foundPeers);

        std::vector<Peer> nearest;
        n.kbucket.findNearestNodes(idToFind, nearest, count);

        std::vector<Peer>::iterator it;
        for(it = nearest.begin(); it != nearest.end(); it ++) {
            addToShortList(*it);
            addToFoundPeers(*it);
        }

        ClosestQueue::iterator i;
        for(i = shortlist.begin(); i != shortlist.end(); i++) {
            sendFindQueryTo((*i).second);
        }
    }

    void FinderActor::treat(FinderActorData& msg) {
        pendingRequests --;
        if(msg.answered) {
            std::vector<Peer>::iterator peerToAdd = msg.answeredPeers->begin();
            for(; peerToAdd != msg.answeredPeers->end(); peerToAdd ++) {
                addToShortList(*peerToAdd);
                addToFoundPeers(*peerToAdd);
            }
        }

        bool sent = sendNewQuery();
        if(!sent) {
            if(pendingRequests == 0) {
                callback->onFound(foundPeers);
                delete callback;
                kill();
            }
        }
    }

    void FinderActor::addToShortList(Peer& p) {
        Distance dist(requestedId, p.id);
        heap_push(shortlist, std::make_pair(dist, p));
        if(shortlist.size() > FIND_REDUNDANCY) {
            heap_pop_no_res(shortlist);
        }
    }

    void FinderActor::addToFoundPeers(Peer& p) {
        Distance dist(requestedId, p.id);
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

    void FinderActor::sendFindQueryTo(Peer& p) {
        addToContactedPeers(p);
        pendingRequests ++;
        new SingularFindActor(n, getId(), p, requestedId);
    }

    bool FinderActor::sendNewQuery() {
        ClosestQueue::iterator it;
        for(it = shortlist.begin(); it != shortlist.end(); it ++) {
            if(contactedPeers.find((*it).first) != contactedPeers.end()) {
                sendFindQueryTo((*it).second);
                return true;
            }
        }
        return false;
    }
}
}
