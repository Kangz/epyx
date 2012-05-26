#include "internal-node.h"
#include "node.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, Node& parent, const std::string& name)
    :actors(5, name + "Actors"), id(id), parent(parent), kbucket(id) {
        pingActor = actors.add(new PingActor(*this));
        getActor = actors.add(new GetActor(*this));
        storeActor = actors.add(new StoreActor(*this));
        findActor = actors.add(new FindActor(*this));
    }

    InternalNode::~InternalNode() {
    }

    void InternalNode::processPacket(Packet& pkt, Target& target) {

        this->kbucket.seenPeer(target.id, target.n2npId);

        switch(pkt.method){
            case M_PING:
                this->pingActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_PONG:
                delete &pkt;
                delete &target;
                break;

            case M_GET:
                this->getActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_GOT:
                this->handleGot(pkt, target);
                break;

            case M_STORE:
                this->storeActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_STORED:
                this->handleStored(pkt, target);
                break;

            case M_FIND:
                this->findActor.post(*(new StaticActorData(target, pkt)));
                break;

            case M_FOUND:
                this->handleFound(pkt, target);
                break;

            default:
                log::error << "The DHT received a packet with an unknown method" << log::endl;
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

    void InternalNode::handleGot(Packet& pkt, Target& target) {
    }

    void InternalNode::handleStored(Packet& pkt, Target& target) {
    }

    void InternalNode::handleFound(Packet& pkt, Target& target) {
    }

}
}
