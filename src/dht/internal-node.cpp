#include "internal-node.h"
#include "node.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, Node& parent, const std::string& name)
    :id(id), parent(parent), kbucket(id) {
    }

    InternalNode::~InternalNode() {
    }

    void InternalNode::processPacket(const N2NP::NodeId& sender, Packet& pkt) {

        this->kbucket.seenPeer(pkt.from, sender);

        switch(pkt.method){
            case M_PING:
                this->sendPong(pkt.from, sender);
                delete &pkt;
                break;

            case M_PONG:
                delete &pkt;
                break;

            case M_GET:
                this->sendGot(pkt, pkt.from, sender);
                break;

            case M_GOT:
                this->handleGot(pkt);
                break;

            case M_STORE:
                this->sendStored(pkt, pkt.from, sender);
                break;

            case M_STORED:
                this->handleStored(pkt);
                break;

            case M_FIND:
                this->sendFound(pkt, pkt.from, sender);
                break;

            case M_FOUND:
                this->handleFound(pkt);
                break;

            default:
                log::error << "The DHT received an unknown method" << log::endl;
                delete &pkt;
                break;
        }
    }

    void InternalNode::sendPong(const Id& target, const N2NP::NodeId& n2npTarget) {
        Packet pkt;
        pkt.method = M_PONG;
        this->parent.send(pkt, target, n2npTarget);
    }

    void InternalNode::sendGot(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_GOT;
        /*if (this->storage.has(pkt.key) {
            answer.value = this->storage.get(pkt.key);
            answer.status = 0;
        }else{
            answer.status = 1;//TODO: Provide further details
        }*/
        this->parent.send(answer, target, n2npTarget);
        delete &pkt;
    }

    void InternalNode::handleGot(Packet& pkt) {
    }

    void InternalNode::sendStored(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_STORED;
        answer.status = 0;
        //TODO check many things
        //this->storage.set(pkt.key, pkt.data);
        this->parent.send(answer, target, n2npTarget);
        delete &pkt;
    }

    void InternalNode::handleStored(Packet& pkt) {
    }

    void InternalNode::sendFound(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget) {
        Packet answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_FOUND;
        this->kbucket.findNearestNodes(pkt.idToFind, answer.foundPeers, pkt.count);

        this->parent.send(answer, target, n2npTarget);
        delete &pkt;
    }

    void InternalNode::handleFound(Packet& pkt) {
    }

}
}
