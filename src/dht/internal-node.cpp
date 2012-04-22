#include "internal-node.h"
#include "node.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, Node& parent, const std::string& name)
    :actors(5, name + "Actors"), id(id), parent(parent), kbucket(id) {
        pingActor = actors.add(new PingActor(*this));
    }

    InternalNode::~InternalNode() {
    }

    void InternalNode::processPacket(Packet& pkt, Target& target) {

        this->kbucket.seenPeer(target.id, target.n2npId);

        switch(pkt.method){
            case M_PING:
                pingActor.post(*(new StaticActorData(target, pkt)));
                delete &pkt;
                break;

            case M_PONG:
                delete &pkt;
                delete &target;
                break;

            case M_GET:
                this->sendGot(pkt, target);
                break;

            case M_GOT:
                this->handleGot(pkt, target);
                break;

            case M_STORE:
                this->sendStored(pkt, target);
                break;

            case M_STORED:
                this->handleStored(pkt, target);
                break;

            case M_FIND:
                this->sendFound(pkt, target);
                break;

            case M_FOUND:
                this->handleFound(pkt, target);
                break;

            default:
                log::error << "The DHT received an unknown method" << log::endl;
                delete &pkt;
                delete &target;
                break;
        }
    }

    void InternalNode::send(Packet& pkt, const Target& target) {
        this->parent.send(pkt, target);
    }

    void InternalNode::sendPong(Target& target) {
        Packet pkt;
        pkt.method = M_PONG;
        this->parent.send(pkt, target);
        delete &target;
    }

    void InternalNode::sendGot(Packet& pkt, Target& target) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_GOT;
        /*if (this->storage.has(pkt.key) {
            answer.value = this->storage.get(pkt.key);
            answer.status = 0;
        }else{
            answer.status = 1;//TODO: Provide further details
        }*/
        this->parent.send(answer, target);
        delete &pkt;
        delete &target;
    }

    void InternalNode::handleGot(Packet& pkt, Target& target) {
    }

    void InternalNode::sendStored(Packet& pkt, Target& target) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_STORED;
        answer.status = 0;
        //TODO check many things
        //this->storage.set(pkt.key, pkt.data);
        this->parent.send(answer, target);
        delete &pkt;
        delete &target;
    }

    void InternalNode::handleStored(Packet& pkt, Target& target) {
    }

    void InternalNode::sendFound(Packet& pkt, Target& target) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_FOUND;
        this->kbucket.findNearestNodes(pkt.idToFind, answer.foundPeers, pkt.count);

        this->parent.send(answer, target);
        delete &pkt;
        delete &target;
    }

    void InternalNode::handleFound(Packet& pkt, Target& target) {
    }

}
}
