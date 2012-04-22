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

        n.processPacket(*pkt, *(new Target(myself, senderId, pkt->from)));
    }

    void Node::send(Packet& pkt, const Target& target) {
        pkt.from = this->id;
        //pkt.to = target.id;
        GTTPacket* gtt = pkt.toGTTPacket();
        target.myN2np.send(target.n2npId, "DHT", *gtt);
    }

}
}
