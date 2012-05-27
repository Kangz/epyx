#include "static-actors.h"
#include "node.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    FinderActorData::FinderActorData(Target& t, std::vector<Peer>* peers, bool ans)
    :target(t), answeredPeers(peers), answered(ans) {
    }

    FinderActorData::~FinderActorData() {
        if(answeredPeers != NULL){
            delete answeredPeers;
        }
        delete &target;
    }

    void FindCallback::onError() {}

    FinderActor::FinderActor(InternalNode& n, Id& idToFind, FindCallback* cb)
    :n(n), requestedId(idToFind), callback(cb) {
    }

    void FinderActor::treat(FinderActorData& msg) {
        
    }

    void FinderActor::timeout() {
        callback.onError();
        delete callback;
        kill();
    }

    void FinderActor::sendFindQueryTo(Target& target) {
        new SingularFindActor(n, getId(), target, requestedId);
    }
}
}
