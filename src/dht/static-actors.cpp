#include "static-actors.h"
#include "node.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    StaticActorData::StaticActorData(Target& target, Packet* pkt)
    :target(target), pkt(pkt) {
    }

    StaticActorData::StaticActorData(Target& target, Packet& pkt)
    :target(target), pkt(&pkt) {
    }

    void StaticActorData::freeData(){
        delete &target;
        delete pkt;
    }

    PingActor::PingActor(InternalNode& n): n(n) {
    }

    void PingActor::treat(StaticActorData& msg) {
        Packet pkt;
        pkt.method = M_PONG;
        this->n.send(pkt, msg.target);
        msg.freeData();
    }

}
}
