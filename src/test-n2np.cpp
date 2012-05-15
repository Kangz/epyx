/**
 * @file test-n2np.cpp
 * @brief This program tests Node to Node Protocol
 */

#include "core/common.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include <strings.h>

/**
 * @brief Command-line interface to send packets
 */
void test_command(Epyx::N2NP::Node& myNode, Epyx::N2NP::NodeId nodeids[], unsigned int nodeNum) {
    unsigned int nodeIndex;
    std::string msg;

    Epyx::log::debug << "[Cli:motd] Command-line interface" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] You are on " << myNode.getId() << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] There are " << nodeNum << " nodes" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] 2 Hello sends `Hello' to node 2" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] 0 quits." << Epyx::log::endl;

    while (true) {
        std::cin >> nodeIndex;
        if (nodeIndex <= 0) {
            Epyx::log::debug << "[Cli] Bye :)" << Epyx::log::endl;
            return;
        } else {
            std::cin >> msg;
            if (nodeIndex > nodeNum) {
                Epyx::log::debug << "[Cli] Too high index. Max is "
                    << nodeNum << Epyx::log::endl;
                continue;
            }
            const Epyx::N2NP::NodeId& nodeid = nodeids[nodeIndex - 1];
            Epyx::log::debug << "[Cli] Sending " << msg << " to " << nodeid << Epyx::log::endl;

            // Send !
            myNode.send(nodeid, "TEST", msg.c_str(), msg.length() + 1);
        }
    }
}
class Ponger :public Epyx::N2NP::Module
{
public:
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
};

void Ponger::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    if (data == NULL) {
        // If no data was received, pong with null
        Epyx::log::debug << "[Node " << node.getId() << "] Recv from " << from
            << ": (null)" << Epyx::log::endl;
        node.send(from, "PONG", NULL, 0);
        return;
    }
    Epyx::log::debug << "[Node " << node.getId() << "] Recv from " << from << ": `"
        << data << "'" << Epyx::log::endl;
    // Send a pong with the same data
    // .. or not
    std::string pongstr(data, size);

    if (!strcasecmp(data, "o<"))
        pongstr = "PAN !";
    else if (!strcasecmp(data, "Question?"))
        pongstr = "The answer is 42.";

    node.send(from, "PONG", pongstr.c_str(), pongstr.length() + 1);
}
class Displayer :public Epyx::N2NP::Module
{
public:
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
};

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    Epyx::log::debug << "[Node " << node.getId() << "] Pong from " << from << ": `"
        << data << "'" << Epyx::log::endl;
}

/**
 * @brief Test local N2NP implementation
 */
void test_n2np() {
    // Create Net Select for relay
    Epyx::NetSelect *selectRelay = new Epyx::NetSelect(10, "WRelay");
    selectRelay->setName("NetSelectRelay");
    selectRelay->start();

    // Create relay
    Epyx::Address addr("127.0.0.1:4242");
    Epyx::N2NP::Relay *relay = new Epyx::N2NP::Relay(addr);
    selectRelay->add(new Epyx::N2NP::RelayServer(new Epyx::TCPServer(addr.getPort(), 50), relay));
    Epyx::log::info << "Start Relay " << relay->getId() << Epyx::log::endl;

    // Create Net Select for nodes
    Epyx::NetSelect *selectNodes = new Epyx::NetSelect(10, "WNodes");
    selectNodes->setName("NetSelectNodes");
    selectNodes->start();

    // Create nodes
    const int nodeNum = 42;
    Epyx::N2NP::Node * nodes[nodeNum];
    Epyx::log::info << "Create nodes..." << Epyx::log::endl;
    Ponger testModule;
    Displayer testModule2;
    for (int i = 0; i < nodeNum; i++) {
        nodes[i] = new Epyx::N2NP::Node(addr);
        nodes[i]->addModule("TEST", &testModule);
        nodes[i]->addModule("PONG", &testModule2);
        //nodes[i]->registerRecv(pongType, nodeRecvPong, NULL);
        selectNodes->add(nodes[i]);
    }

    // Wait for node IDs
    Epyx::log::info << "Waiting for nodes..." << Epyx::log::endl;
    Epyx::N2NP::NodeId nodeids[nodeNum];
    for (int i = 0; i < nodeNum; i++) {
        while (!nodes[i]->isReady())
            usleep(100);
        nodeids[i] = nodes[i]->getId();
    }

    test_command(*(nodes[0]), nodeids, nodeNum);

    // Everything stops at destruction, but nodes first !
    delete selectNodes;
    Epyx::log::info << "Waiting for nodes to be detached..." << Epyx::log::endl;
    if (!relay->waitForAllDetach(Epyx::Timeout(2000))) {
        Epyx::log::info << "Detach remaining nodes" << Epyx::log::endl;
        relay->detachAllNodes();
    }
    delete selectRelay;
    delete relay;
}

/**
 * @brief Setup environment and invoke test_n2np()
 */
int main() {
    try {
        Epyx::Thread::init();
        Epyx::log::init(Epyx::log::CONSOLE, "");
        test_n2np();
        Epyx::log::flushAndQuit();
    } catch (Epyx::Exception e) {
        std::cerr << e << "\n";
    }
    return 0;
}
