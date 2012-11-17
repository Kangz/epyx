#include "internal-node.h"
#include "node.h"
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, N2NP::Node& n2npSelf, Node& parent, const std::string& name)
    :actors(5, name + "Actors"), id(id), myN2np(n2npSelf), 
    myPeer(new Peer(id, myN2np.getId())), parent(parent), kbucket(id),

    //Create the actors that will respond to simple requests
    pingActor(actors.add(new PingActor(*this))),
    getActor(actors.add(new GetActor(*this))),
    storeActor(actors.add(new StoreActor(*this))),
    findActor(actors.add(new FindActor(*this))) {
    }

    InternalNode::~InternalNode() {}

    void InternalNode::processPacket(Packet* origPkt, const Peer& origSender) {
        //Update the routing table
        Peer::SPtr sender = this->kbucket.seenPeer(origSender.id, origSender.n2npId);

        Packet::SPtr pkt(origPkt);

        //Dispach the packet to the right actor
        switch(pkt->method){
            case M_PING:
                this->pingActor.post(sender, pkt);
                break;

            case M_PONG:
                break;

            case M_GET:
                this->getActor.post(sender, pkt);
                break;

            case M_STORE:
                this->storeActor.post(sender, pkt);
                break;

            case M_FIND:
                this->findActor.post(sender, pkt);
                break;

            case M_GOT:
            case M_STORED:
            case M_FOUND:
                this->dispatchToProcessActor(pkt, sender);
                break;

            default:
                log::error << "The DHT received a packet with an unknown method" << log::endl;
                break;
        }
    }

    void InternalNode::send(Packet& pkt, Peer::SPtr dest) {
        this->parent.send(pkt, dest, myN2np);
    }

    void InternalNode::sendPing(Peer::SPtr p){
        Packet pkt;
        pkt.method = M_PING;
        send(pkt, p);
    }

    Peer::SPtr InternalNode::getConnectionInfo() const {
        return myPeer;
    }

    //These methods spawn actors for the "long" operations
    void InternalNode::findClosest(FindCallback* cb, int count, const Id& idToFind) {
        FinderActor* a = new FinderActor(*this, idToFind, count, cb);
        actors.add(a, FIND_CALLBACK_TIMEOUT);
        a->start();
    }

    void InternalNode::getValue(GetCallback* cb, const std::string& key) {
        GetterActor* a = new GetterActor(*this, key, cb);
        actors.add(a, GET_CALLBACK_TIMEOUT);
        a->start();
    }

    void InternalNode::setValue(SetCallback* cb, const std::string& key, const std::string& value) {
        SetterActor* a = new SetterActor(*this, key, value, cb);
        actors.add(a, SET_CALLBACK_TIMEOUT);
        a->start();
    }

    long InternalNode::registerNewConnectionId(ActorId<ProcessActor> actor) {
        long n = std::atomic_fetch_add(&processActorsCount, 1L);
        processActors.set(n, actor);
        return n;
    }

    void InternalNode::dispatchToProcessActor(Packet::SPtr pkt, Peer::SPtr sender){
        auto it = processActors.findAndLock(pkt->connectionId);
        if(processActors.isEnd(it)){
            processActors.endUnlock();
            return;
        }

        ActorId<ProcessActor> saved_id((*it).second);
        processActors.endUnlock();

        saved_id.post(EPYX_AQ("process receive"), sender, pkt);
    }

    void InternalNode::unregisterConnectionId(long id) {
        processActors.unset(id);
    }

}
}
