#include <mutex>
#include <condition_variable>
#include "node.h"

namespace Epyx
{
namespace DHT
{

    Node::Node(const Id& id, N2NP::Node& n2npSelf, const std::string& name)
    : id(id), n(id, n2npSelf, *this, name) {
    }

    Node::~Node() {
    }

    void Node::fromN2NP(N2NP::Node& myself, N2NP::NodeId senderId, const byte_str& data) {
        //Preprocess the packet for the InternalNode
        //first make it a GTTPacket
        gttParser.eat(data);
        std::unique_ptr<GTTPacket> gtt_packet = gttParser.getPacket();
        gttParser.reset();

        if(!gtt_packet){
            log::error << "The DHT received an incomplete packet" << log::endl;
            return;
        }

        //TODO: check that the packet is well-formed
        //Then make it a DHT Packet
        Packet* pkt = new Packet(*gtt_packet);
        Peer peer(pkt->from, senderId);

        n.processPacket(pkt, peer);
    }

    void Node::send(Packet& pkt, Peer::SPtr dest, N2NP::Node& myN2np) {
        //We just need to add ourself to the packet
        pkt.from = this->id;

        //Convert it
        GTTPacket* gtt = pkt.toGTTPacket();

        //And send it
        myN2np.send(dest->n2npId, "DHT", *gtt);
    }

    //These are simply proxies for the InternalNode
    void Node::sendPing(Peer::SPtr p){
        n.sendPing(p);
    }

    Peer::SPtr Node::getConnectionInfo() const {
        return n.getConnectionInfo();
    }

    void Node::findClosest(FindCallback* cb, int count, const Id& idToFind) {
        n.findClosest(cb, count, idToFind);
    }

    void Node::getValue(GetCallback* cb, const std::string& key) {
        n.getValue(cb, key);
    }

    void Node::setValue(SetCallback* cb, const std::string& key, const std::string& value) {
        n.setValue(cb, key, value);
    }


    //The synchronous API is using the asynchronous API internally
    class NodeSyncGetCallback: public GetCallback {
    private:
        std::condition_variable* cond;
        std::string* whereResultIsStored;
        bool* got;
    public:
        NodeSyncGetCallback(std::condition_variable* c, std::string* storage, bool* b) {
            cond = c;
            got = b;
            whereResultIsStored = storage;
        }
        virtual void onGot(const std::string& result) {
            *got = true;
            *whereResultIsStored = result;
            cond->notify_one();
        }
        virtual void onError() {
            *got = false;
            cond->notify_one();
        }
    };

    class NodeSyncSetCallback: public SetCallback {
    private:
        std::condition_variable* cond;
        bool* set;
    public:
        NodeSyncSetCallback(std::condition_variable* c, bool* b) {
            cond = c;
            set = b;
        }
        virtual void onSet() {
            *set = true;
            cond->notify_one();
        }
        virtual void onError() {
            *set = false;
            cond->notify_one();
        }
    };

    bool Node::getValueSync(const std::string& key, std::string& result) {
        std::condition_variable cond;
        bool got;
        {
            std::mutex m;
            std::unique_lock<std::mutex> lock(m);
            getValue(new NodeSyncGetCallback(&cond, &result, &got), key);
            cond.wait(lock);
        }
        return got;
    }

    bool Node::setValueSync(const std::string& key, const std::string& value) {
        std::condition_variable cond;
        bool set;
        {
            std::mutex m;
            std::unique_lock<std::mutex> lock(m);
            setValue(new NodeSyncSetCallback(&cond, &set), key, value);
            cond.wait(lock);
        }
        return set;
    }

}
}
