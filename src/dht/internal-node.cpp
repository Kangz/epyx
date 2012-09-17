#include "internal-node.h"
#include "node.h"
#include "consts.h"

namespace Epyx
{
namespace DHT
{

    InternalNode::InternalNode(const Id& id, N2NP::Node& n2npSelf, Node& parent, const std::string& name)
    :actors(5, name + "Actors"), id(id), myN2np(n2npSelf), parent(parent), kbucket(id),

    //Create the actors that will respond to simple requests
    pingActor(actors.add(new PingActor(*this))),
    getActor(actors.add(new GetActor(*this))),
    storeActor(actors.add(new StoreActor(*this))),
    findActor(actors.add(new FindActor(*this))) {
    }

    InternalNode::~InternalNode() {
        //TODO
    }

    void InternalNode::processPacket(Packet& pkt, Peer& sender) {

        //Update the routing table
        this->kbucket.seenPeer(sender.id, sender.n2npId);

        //Dispach the packet to the right actor
        switch(pkt.method){
            case M_PING:
                this->pingActor.post(new StaticActorData(sender, pkt));
                break;

            case M_PONG:
                delete &pkt;
                delete &sender;
                break;

            case M_GET:
                this->getActor.post(new StaticActorData(sender, pkt));
                break;

            case M_STORE:
                this->storeActor.post(new StaticActorData(sender, pkt));
                break;

            case M_FIND:
                this->findActor.post(new StaticActorData(sender, pkt));
                break;

            case M_GOT:
            case M_STORED:
            case M_FOUND:
                this->dispatchToProcessActor(pkt, sender);
                break;

            default:
                log::error << "The DHT received a packet with an unknown method" << log::endl;
                delete &pkt;
                delete &sender;
                break;
        }
    }

    void InternalNode::send(Packet& pkt, const Peer& dest) {
        this->parent.send(pkt, dest, myN2np);
    }

    void InternalNode::sendPing(Peer& p){
        Packet pkt;
        pkt.method = M_PING;
        send(pkt, p);
    }

    Peer InternalNode::getConnectionInfo() {
        return Peer(id, myN2np.getId());
    }

    //These methods spawn actors for the "long" operations
    void InternalNode::findClosest(FindCallback* cb, int count, Id& idToFind) {
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

    //These methods manage the process actors
    long InternalNode::registerProcessActor(ProcessActor& actor, int timeout) {
        //TODO delete all this when the node is destroyed
        ActorId<ProcessActor>* a;
        if(timeout > 0){
            a = new ActorId<ProcessActor>(actors.add(actor, timeout));
        }else{
            a = new ActorId<ProcessActor>(actors.add(actor));
        }
        long n = processActorsCount.getIncrement();
        processActors.set(n, a);
        return n;
    }

    void InternalNode::dispatchToProcessActor(Packet& pkt, Peer& sender){
        ActorId<ProcessActor>* id = processActors.getAndLock(pkt.connectionId, NULL);
        if(id == NULL){
            processActors.endUnlock();
            return;
        }
        ActorId<ProcessActor> saved_id(*id);
        processActors.endUnlock();

        saved_id.post(new ProcessActorData(sender, pkt));
    }

    void InternalNode::unregisterProcessActor(long actorNumber){
        ActorId<ProcessActor>* id = processActors.getAndLock(actorNumber, NULL);
        if(id == NULL){
            processActors.endUnlock();
            return;
        }
        ActorId<ProcessActor> saved_id(*id);
        delete id;
        processActors.endUnlock();

        saved_id.kill();
        processActors.unset(actorNumber);
    }

}
}
