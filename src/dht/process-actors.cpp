#include "process-actors.h"
#include "internal-node.h"

namespace Epyx
{
namespace DHT
{

    ProcessActorData::ProcessActorData(Target& target, Packet* pkt)
    :target(target), pkt(pkt) {
    }

    ProcessActorData::ProcessActorData(Target& target, Packet& pkt)
    :target(target), pkt(&pkt) {
    }

    void ProcessActorData::freeData(){
        delete &target;
        delete pkt;
    }
}
}
