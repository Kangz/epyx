#include "node.h"

namespace Epyx
{
namespace DHT
{

    Node::Node(Id& id, const std::string& name): id(id), n(id, *this, name) {
    }

    Node::~Node() {
    }

    void Node::eatN2NP(const N2NP::NodeId& senderId, const N2NP::NodeId& myself, char* data, int dataSize) {
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

        n.processPacket(senderId, *pkt);
    }

    void Node::send(Packet& pkt, const Id& target, const N2NP::NodeId& n2npTarget) {
        pkt.from = this->id;
        GTTPacket* gtt = pkt.toGTTPacket();
        //std::string& raw_data = gtt->serialize();
        //this->n2np.send(n2npTarget, "dht", raw_data.c_str(), raw_data.length());
    }

}
}
