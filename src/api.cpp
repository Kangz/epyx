#include <stack>


#include "natpunching/module.h"

#include "api.h"
#include "natpunching/openconnection.h"

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
        {
            std::lock_guard<std::mutex> lock(mut);
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
        }

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

    void API::spawnRelay(const SockAddress& addr, unsigned int nbConn) {
        if (netsel == NULL)
            throw FailException("API::spawnRelay", "No NetSelect created");

        if (relay != NULL)
            throw FailException("API::spawnRelay", "Spawning multiple relays");

        {
            std::lock_guard<std::mutex> lock(mut);
            relay = new N2NP::Relay(addr);
            //nsRelayId = netsel->add(new N2NP::RelayServer(new TCPServer(addr, nbConn), relay));
            nsRelayId = netsel->add(new N2NP::RelayServer(new TCPServer(SockAddress("0.0.0.0", addr.getPort()), nbConn), relay));
        }
    }

    void API::destroyRelay(const Timeout& detachTime) {
        if (netsel == NULL)
            throw FailException("API::destroyRelay", "No NetSelect created");

        if (relay == NULL)
            return;

        {
            std::lock_guard<std::mutex> lock(mut);
            if (!relay->waitForAllDetach(detachTime)) {
                log::info << "Detach remaining nodes" << log::endl;
                relay->detachAllNodes();
            }
            netsel->kill(nsRelayId);
            delete relay;
            relay = NULL;
        }
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

    API::OpenConnThread::OpenConnThread(API *api)
    :api(api) {
        EPYX_ASSERT(api != NULL);
    }

    void API::OpenConnThread::run() {
        EPYX_ASSERT(api != NULL);
        while (true) {
            // Get node indexes from the locked map
            std::list<int> nodeIndexesList;
            for (atom::Map<int, int>::const_iterator i = api->nodeIndexes.beginLock();
                !api->nodeIndexes.isEnd(i); i++) {
                nodeIndexesList.push_back(i->second);
            }
            api->nodeIndexes.endUnlock();

            // Treat each node
            for (std::list<int>::const_iterator i = nodeIndexesList.begin();
                i != nodeIndexesList.end(); i++) {
                NetSelectReader *nodeAsReader = api->netsel->get(*i);
                N2NP::Node *node = static_cast<N2NP::Node*> (nodeAsReader);
                std::stack<N2NP::NodeId> idLists;
                node->askForDirectConnectionIds(idLists);
                while (!idLists.empty()) {
                    N2NP::NodeId id = idLists.top();
                    idLists.pop();
                    DirectConnection::Module::openDirectConnection(*node, id);
                    sleep(2);
                }
            }
        }
    }
}
