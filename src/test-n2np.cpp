/**
 * @file test-n2np.cpp
 * @brief This program tests Node to Node Protocol
 */

#include "api.h"
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
void test_n2np(Epyx::API& epyx, const Epyx::SockAddress &addr) {
    // Create relay
    epyx.spawnRelay(addr);
    Epyx::log::info << "Start Relay at " << addr << Epyx::log::endl;

    // Create nodes
    const int nodeNum = 42;
    Epyx::N2NP::Node* firstNode = NULL;
    Epyx::N2NP::NodeId nodeids[nodeNum];
    Epyx::log::info << "Create nodes..." << Epyx::log::endl;
    Ponger testModule;
    Displayer testModule2;
    for (int i = 0; i < nodeNum; i++) {
        Epyx::N2NP::Node *node = new Epyx::N2NP::Node(addr);
        if (firstNode == NULL)
            firstNode = node;
        node->addModule("TEST", &testModule);
        node->addModule("PONG", &testModule2);
        //node->registerRecv(pongType, nodeRecvPong, NULL);
        epyx.addNode(node);
        if (!node->waitReady(5000)) {
            delete node;
            Epyx::log::error << "Initialisation of node " << i << " took too much time"
                << Epyx::log::endl;
            epyx.destroyAllNodes();
            epyx.destroyRelay(2000);
            return;
        }
        nodeids[i] = node->getId();
    }

    test_command(*firstNode, nodeids, nodeNum);

    Epyx::log::info << "Destroy everything for " << addr << Epyx::log::endl;
    epyx.destroyAllNodes();
    epyx.destroyRelay(2000);
}

/**
 * @brief Setup environment and invoke test_n2np()
 */
int main() {
    Epyx::API epyx;
    try {
        epyx.setNetWorkers(50);
        test_n2np(epyx, Epyx::SockAddress("127.0.0.1:4242"));
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
