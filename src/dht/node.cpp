#include "node.h"

namespace Epyx
{
namespace DHT
{

    Node::Node(){
        this->id = id;
        this->kbucket = new KBucket(&id);
    }

    Node::~Node(){
    }

    void Node::eatN2NP(N2npNodeId senderId, N2npNodeId myself, char* data, int dataSize){
        gttParser.eat(data, dataSize);
        GTTPacket* gtt_packet = gttParser.getPacket();
        gttParser.reset();

        if(gtt_packet == NULL){
            log::error << "The DHT received an incomplete packet" << log::endl;
            return;
        }

        DHTPacket pkt(*gtt_packet);
        delete gtt_packet;

        this->kbucket->seenPeer(&pkt.from, senderId);

        switch(pkt.method){
            case M_PING:
                this->sendPong(pkt.from, senderId);
                break;

            case M_PONG:
                break;

            case M_GET:
                this->sendGot(pkt, pkt.from, senderId);
                break;

            case M_GOT:
                this->handleGot(pkt);
                break;

            case M_STORE:
                this->sendStored(pkt, pkt.from, senderId);
                break;

            case M_STORED:
                this->handleStored(pkt);
                break;

            case M_FIND:
                this->sendFound(pkt, pkt.from, senderId);
                break;

            case M_FOUND:
                this->handleFound(pkt);
                break;

            default:
                log::error << "The DHT received an unknown method" << log::endl;
                break;
        }
    }

    void Node::send(DHTPacket& pkt, Id target, N2npNodeId n2npTarget){
        pkt.from = this->id;
        GTTPacket* gtt = pkt.toGTTPacket();
        //std::string& raw_data = gtt->serialize();
        //this->n2np.send(n2npTarget, "dht", raw_data.c_str(), raw_data.length());
    }

    void Node::sendPong(Id target, N2npNodeId n2npTarget){
        DHTPacket pkt;
        pkt.method = M_PONG;
        this->send(pkt, target, n2npTarget);
    } 

    void Node::sendGot(DHTPacket& pkt, Id target, N2npNodeId n2npTarget){
        DHTPacket answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_GOT;
        /*if (this->storage.has(pkt.key) {
            answer.value = this->storage.get(pkt.key);
            answer.status = 0; 
        }else{
            answer.status = 1;//TODO: Provide further details
        }*/
        this->send(answer, target, n2npTarget);
    }

    void Node::handleGot(DHTPacket& pkt){
    }

    void Node::sendStored(DHTPacket& pkt, Id target, N2npNodeId n2npTarget){
        DHTPacket answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_STORED;
        answer.status = 0;
        //TODO check many things
        //this->storage.set(pkt.key, pkt.data);
        this->send(answer, target, n2npTarget);
    }

    void Node::handleStored(DHTPacket& pkt){
    }

    void Node::sendFound(DHTPacket& pkt, Id target, N2npNodeId n2npTarget){
        DHTPacket answer;
        answer.connectionId = pkt.connectionId;
        answer.method = M_FOUND;
        this->kbucket->findNearestNodes(pkt.idToFind, answer.foundPeers, pkt.count);
         
        this->send(answer, target, n2npTarget);
    }

    void Node::handleFound(DHTPacket& pkt){
    }

}
}
