#include <stack>


#include "natpunching/module.h"

#include "api.h"
#include "natpunching/openconnection.h"
#include "natpunching/dispatcher.h"

namespace Epyx
{

    API::API(int logflags, const std::string& logfile)
    :netselThread(nullptr), nsRelayId(0) {
        // Number of workers in NetSelect may be configured via netsel.setNumWorkers
        Thread::init();
        log::init(logflags, logfile);
        Socket::init();
    }

    API::~API() {
        this->destroyAllNodes();
        this->destroyRelay(2000);
        if (netsel) {
            netsel->stop();
        }
        if (netselThread) {
            netselThread->join();
            delete netselThread;
            netselThread = nullptr;
        }
        log::flushAndQuit();
    }

    void API::setNetWorkers(int numworkers) {
        std::lock_guard<std::mutex> lock(mut);
        if (!netsel) {
            netsel.reset(new NetSelect(numworkers, "NetSelectWorker"));
            netselThread = new std::thread(&NetSelect::runLoop, netsel.get(), "NetSelect");
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
        DirectConnection::Dispatcher::addModule(node);
        return node;
    }

    std::shared_ptr<DHT::Node> API::createDHTNode(const std::string& name,
        std::shared_ptr<N2NP::Node> node, bool first) {
        EPYX_ASSERT(node && !name.empty());

        // Create a DHT node with a random ID
        DHT::Id id(first ? DHT::Id::INIT_ZERO : DHT::Id::INIT_RANDOM);
        std::shared_ptr<DHT::Node> dhtNode(new DHT::Node(id, *node, name));
        node->addModule("DHT", dhtNode);
        return dhtNode;
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
        EPYX_ASSERT(netsel && netselThread);
        netselThread->join();
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
                std::shared_ptr<N2NP::Node> node = std::static_pointer_cast<N2NP::Node> (*i);
                std::stack<N2NP::NodeId> idLists;
                node->askForDirectConnectionIds(idLists);
                while (!idLists.empty()) {
                    N2NP::NodeId id = idLists.top();
                    idLists.pop();
                    DirectConnection::Module::openDirectConnection(node, id);
                    sleep(2);
                }
            }
        }
    }
}
