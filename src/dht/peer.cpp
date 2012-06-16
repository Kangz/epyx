#include "peer.h"
#include <sstream>

namespace Epyx
{
namespace DHT
{

    const char DHT_FOUND_PEERS_DELIM = '%';

    Peer::Peer()
    :lastReceiveTime(0) {
    }

    Peer::Peer(const Id& id, const N2NP::NodeId& n2npId, time_t lastReceiveTime)
    :id(id), n2npId(n2npId), lastReceiveTime(lastReceiveTime) {
    }

    Peer::Peer(const N2NP::NodeId& n2npId)
    :n2npId(n2npId) {
    }

    //TODO after we have a serialize/unserialize for N2npNodeId

    void Peer::serialize(std::ostream& o) {
        o << n2npId;
        o << DHT_FOUND_PEERS_DELIM;
        o << id;
    }

    void Peer::unserialize(std::istream& i) {
        std::stringbuf sb;
        i.get(sb, DHT_FOUND_PEERS_DELIM);
        n2npId = N2NP::NodeId(sb.str());

        i.ignore(); //remove the delimiter
        i >> id;
    }

}
}
