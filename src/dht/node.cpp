#include "node.h"

namespace Epyx
{
namespace DHT
{

    Node::Node(Id& id, N2NP::Node& n2npSelf, const std::string& name): id(id), n(id, n2npSelf, *this, name) {
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

    void Node::sendPing(Peer& p){
        n.sendPing(p);
    }

    Peer Node::getConnectionInfo() {
        return n.getConnectionInfo();
    }

    void Node:: findClosest(FindCallback* cb, int count, Id& idToFind) {
        n.findClosest(cb, count, idToFind);
    }

    void Node::getValue(GetCallback* cb, const std::string& key) {
        n.getValue(cb, key);
    }

    void Node::setValue(SetCallback* cb, const std::string& key, const std::string& value) {
        n.setValue(cb, key, value);
    }

    class NodeSyncGetCallback: public GetCallback {
        Condition* cond;
        std::string* whereResultIsStored;
        bool* got;
        public:
            NodeSyncGetCallback(Condition* c, std::string* storage, bool* b) {
                cond = c;
                got = b;
                whereResultIsStored = storage;
            }
            void onGot(const std::string& result) {
                *got = true;
                *whereResultIsStored = result;
                cond->lock();
                cond->notify();
                cond->unlock();
            }
            void onError() {
                *got = false;
                cond->lock();
                cond->notify();
                cond->unlock();
            }
    };

    class NodeSyncSetCallback: public SetCallback {
        Condition* cond;
        bool* set;
        public:
                NodeSyncSetCallback(Condition* c, bool* b) {
                    cond = c;
                    set = b;
                }
                void onSet() {
                    *set = true;
                    cond->lock();
                    cond->notify();
                    cond->unlock();
                }
                void onError() {
                    *set = false;
                    cond->lock();
                    cond->notify();
                    cond->unlock();
                }
    };

    bool Node::getValueSync(const std::string& key, std::string& result) {
        Condition cond;
        bool got;
        cond.lock();
        getValue(new NodeSyncGetCallback(&cond, &result, &got), key);
        cond.wait();
        cond.unlock();
        return got;
    }

    bool Node::setValueSync(const std::string& key, const std::string& value) {
        Condition cond;
        bool set;
        cond.lock();
        setValue(new NodeSyncSetCallback(&cond, &set), key, value);
        cond.wait();
        cond.unlock();
        return set;
    }

}
}
