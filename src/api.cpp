#include "api.h"

namespace Epyx
{

    API::API(int logflags, const std::string& logfile)
    :netsel(NULL), relay(NULL), nsRelayId(0) {
        // Number of workers in NetSelect may be configured via netsel.setNumWorkers
        Thread::init();
        log::init(logflags, logfile);
        Socket::init();
    }

    API::~API() {
        mut.lock();
        // Destroy relay, if it exists
        if (relay != NULL) {
            if (netsel != NULL)
                netsel->kill(nsRelayId);
            delete relay;
            relay = NULL;
        }

        // Destroy NetSelect
        if (netsel != NULL) {
            delete netsel;
            netsel = NULL;
        }
        mut.unlock();

        log::flushAndQuit();
    }

    void API::setNetWorkers(int numworkers) {
        mut.lock();
        if (netsel == NULL) {
            netsel = new NetSelect(numworkers, "NetSelectWorker");
            netsel->setName("NetSelect");
            if (!netsel->start()) {
                mut.unlock();
                throw FailException("API::NetSelect", "failed to start");
            }
        } else {
            netsel->setNumWorkers(numworkers);
        }
        mut.unlock();
    }

    void API::spawnRelay(const Address& addr, unsigned int nbConn) {
        if (netsel == NULL)
            throw FailException("API::spawnRelay", "No NetSelect created");

        if (relay != NULL)
            throw FailException("API::spawnRelay", "Spawning multiple relays");

        mut.lock();
        relay = new N2NP::Relay(addr);
        nsRelayId = netsel->add(new N2NP::RelayServer(new TCPServer(addr, nbConn), relay));
        mut.unlock();
    }

    void API::destroyRelay(const Timeout& detachTime) {
        if (netsel == NULL)
            throw FailException("API::destroyRelay", "No NetSelect created");

        if (relay == NULL)
            return;

        mut.lock();
        if (!relay->waitForAllDetach(detachTime)) {
            log::info << "Detach remaining nodes" << log::endl;
            relay->detachAllNodes();
        }
        netsel->kill(nsRelayId);
        delete relay;
        relay = NULL;
        mut.unlock();
    }

    int API::addNode(N2NP::Node *node) {
        if (netsel == NULL)
            throw FailException("API::addNode", "No NetSelect created");
        EPYX_ASSERT(node != NULL);

        int index = netsel->add(node);
        nodeIndexes.set(index, index);
        return index;
    }

    void API::destroyNode(int nodeId) {
        if (netsel == NULL)
            throw FailException("API::killNode", "No NetSelect created");
        nodeIndexes.unset(nodeId);
        netsel->kill(nodeId);
    }

    void API::destroyAllNodes() {
        EPYX_ASSERT(netsel != NULL);
        for (atom::Map<int, int>::iterator i = nodeIndexes.beginLock();
            !nodeIndexes.isEnd(i); i++) {
            netsel->kill(i->first);
        }
        nodeIndexes.clear(true);
        nodeIndexes.endUnlock();
    }

    void API::waitNet() {
        EPYX_ASSERT(netsel != NULL);
        netsel->wait();
    }
}
