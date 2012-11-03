#include "process-actors.h"
#include "internal-node.h"
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    ProcessActor::ProcessActor(InternalNode& n): n(n) {}

    ProcessActor::~ProcessActor() {
        for (auto it = queries.begin(); it != queries.end(); it ++) {
            this->n.unregisterConnectionId((*it).first);
        }
    }

    void ProcessActor::treat(EPYX_AQA("process receive"), Peer* peer, Packet* pkt) {
        auto it = queries.find(pkt->connectionId);
        if (it == queries.end() || (! (*it).second)) {
            return;
        }

        (*it).second = false;
        this->n.unregisterConnectionId((*it).first);
        this->onNewAnswer(peer, pkt);
    }

    void ProcessActor::timeout(EPYX_AQA("process timeout"), long pNumber) {
        auto it = queries.find(pNumber);
        if (it == queries.end()) {
            return;
        }

        (*it).second = false;
        this->n.unregisterConnectionId((*it).first);
        this->onAnswerTimeout(pNumber);
    }

    long ProcessActor::sendQuery(Peer::SPtr peer, Packet& pkt, Timeout timeout) {
        auto selfId = getId(this);
        long connectionId = this->n.registerNewConnectionId(selfId);
        queries[connectionId] = true;
        pkt.connectionId = connectionId;

        //HACK: use shared pointer instead
        Peer target(*peer);

        this->n.send(pkt, target);

        selfId.timeout(timeout, EPYX_AQ("process timeout"), connectionId);
        return connectionId;
    }

    void ProcessActor::onAnswerTimeout(long id) {}

}
}
