#include <stack>


#include "natpunching/module.h"

#include "api.h"
#include "natpunching/openconnection.h"

namespace Epyx
{

    API::API(int logflags, const std::string& logfile)
    :nsRelayId(0) {
        // Number of workers in NetSelect may be configured via netsel.setNumWorkers
        Thread::init();
        log::init(logflags, logfile);
        Socket::init();
    }

    API::~API() {
        this->destroyAllNodes();
        this->destroyRelay(2000);
        log::flushAndQuit();
    }

    void API::setNetWorkers(int numworkers) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel) {
            netsel.reset(new NetSelect(numworkers, "NetSelectWorker"));
            netsel->setThreadName("NetSelect");
            if (!netsel->start()) {
                throw FailException("API::NetSelect", "failed to start");
            }
        } else {
            netsel->setNumWorkers(numworkers);
        }
    }

    void API::spawnRelay(const SockAddress& addr, unsigned int nbConn) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel)
            throw FailException("API::spawnRelay", "No NetSelect created");

        if (relay)
            throw FailException("API::spawnRelay", "Spawning multiple relays");

        relay.reset(new N2NP::Relay(addr));
        SockAddress listenAddr("0.0.0.0", addr.getPort());
        std::shared_ptr<N2NP::RelayServer> srv(new N2NP::RelayServer(new TCPServer(listenAddr, nbConn), relay));
        nsRelayId = netsel->add(srv);
    }

    void API::destroyRelay(int msec) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel || !relay)
            return;

        if (!relay->waitForAllDetach(msec)) {
            log::info << "Detach remaining nodes" << log::endl;
            relay->detachAllNodes();
        }
        netsel->kill(nsRelayId);
        relay.reset();
    }

    int API::addNode(std::shared_ptr<N2NP::Node>& node) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel)
            throw FailException("API::addNode", "No NetSelect created");
        EPYX_ASSERT(node);
        int index = netsel->add(node);
        nodeIndexes[index] = index;
        return index;
    }

    std::shared_ptr<N2NP::Node> API::spawnN2NPNode(const SockAddress& addr, int timeout) {
        std::shared_ptr<N2NP::Node> node(new N2NP::Node(addr));
        this->addNode(node);

        // Wait until connection
        if (timeout > 0 && !node->waitReady(timeout)) {
            throw FailException("API::spawnN2NPNode", "Node initialisation timed out");
        }
        return node;
    }

    void API::destroyNode(int nodeId) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel)
            throw FailException("API::destroyNode", "No NetSelect created");
        nodeIndexes.erase(nodeId);
        netsel->kill(nodeId);
    }

    void API::destroyAllNodes() {
        std::lock_guard<std::mutex> lock(mut);
        if (netsel) {
            for (auto i = nodeIndexes.begin(); i != nodeIndexes.end(); i++) {
                netsel->kill(i->first);
            }
        }
        nodeIndexes.clear();
    }

    void API::waitNet() {
        std::lock_guard<std::mutex> lock(mut);
        EPYX_ASSERT(netsel);
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
            std::list<std::shared_ptr<NetSelectReader> > nodesList;
            {
                std::lock_guard<std::mutex> lock(api->mut);
                for (auto i = api->nodeIndexes.begin(); i != api->nodeIndexes.end(); i++) {
                    std::shared_ptr<NetSelectReader> nodeAsReader = api->netsel->get(i->second);
                    if (nodeAsReader) {
                        nodesList.push_back(nodeAsReader);
                    }
                }
            }

            // Treat each node
            for (auto i = nodesList.begin(); i != nodesList.end(); i++) {
                N2NP::Node *node = static_cast<N2NP::Node*> (i->get());
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
