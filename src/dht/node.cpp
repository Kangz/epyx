#include "node.h"

namespace Epyx
{
namespace DHT
{

    Node::Node(Id& id, const std::string& name): id(id), n(id, *this, name) {
    }

    Node::~Node() {
    }

    void Node::fromN2NP(N2NP::Node& myself, N2NP::NodeId senderId, const char* data, unsigned int dataSize) {
        gttParser.eat(data, dataSize);
        GTTPacket* gtt_packet = gttParser.getPacket();
        gttParser.reset();

        if(gtt_packet == NULL){
            log::error << "The DHT received an incomplete packet" << log::endl;
            return;
        }

        //TODO: check that the packet is well-formed
        Packet* pkt = new Packet(*gtt_packet);
        delete gtt_packet;

        n.processPacket(myself, senderId, *pkt);
    }

    void Node::send(N2NP::Node& n2npNode, Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget) {
        pkt.from = this->id;
        GTTPacket* gtt = pkt.toGTTPacket();
        //std::string& raw_data = gtt->build();
        n2npNode.send(n2npTarget, "DHT", *gtt);
    }

}
}
