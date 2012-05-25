#include "api.h"

namespace Epyx
{

    API::API(int logflags, const std::string& logfile)
    :netsel(NULL), relay(NULL), nsRelayId(0) {
        // Number of workers in NetSelect may be configured via netsel.setNumWorkers
        Thread::init();
        log::init(logflags, logfile);
    }

    API::~API() {
        // Destroy relay, if it exists
        if (relay != NULL) {
            if (netsel != NULL)
                netsel->kill(nsRelayId);
            delete relay;
            relay = NULL;
        }

        // Destroy NetSelect
        if (netsel != NULL)
            delete netsel;

        log::flushAndQuit();
    }

    void API::setNetWorkers(int numworkers) {
        if (netsel == NULL) {
            netsel = new NetSelect(numworkers, "NetSelectWorker");
            netsel->setName("NetSelect");
            if (!netsel->start())
                throw FailException("API::NetSelect", "failed to start");
        } else {
            netsel->setNumWorkers(numworkers);
        }
    }

    void API::spawnRelay(const Address& addr, unsigned int nbConn) {
        if (netsel == NULL)
            throw FailException("API::spawnRelay", "No NetSelect created");

        relay = new N2NP::Relay(addr);
        nsRelayId = netsel->add(new N2NP::RelayServer(new TCPServer(addr.getPort(), nbConn), relay));
    }

    void API::destroyRelay(const Timeout& detachTime) {
        if (netsel == NULL)
            throw FailException("API::destroyRelay", "No NetSelect created");

        if (relay == NULL)
            return;

        if (!relay->waitForAllDetach(detachTime)) {
            log::info << "Detach remaining nodes" << log::endl;
            relay->detachAllNodes();
        }
        netsel->kill(nsRelayId);
        delete relay;
        relay = NULL;
    }

    int API::addNode(N2NP::Node *node) {
        if (netsel == NULL)
            throw FailException("API::addNode", "No NetSelect created");
        EPYX_ASSERT(node != NULL);
            
        return netsel->add(node);
    }
    
    void API::destroyNode(int nodeId) {
        if (netsel == NULL)
            throw FailException("API::killNode", "No NetSelect created");
        netsel->kill(nodeId);
    }
}
