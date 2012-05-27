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

    FinderActor::FinderActor(InternalNode& n, Id& idToFind)
    :n(n), requestId(idToFind) {}

    void FinderActor::treat(FinderActorData& msg) {
    }

    void FinderActor::timeout() {
    }

    void sendFindQueryTo(Target& target) {
    }
}
}
