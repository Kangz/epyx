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
void test_n2np(Epyx::API& epyx, const Epyx::Address &addr) {
    // Create relay
    epyx.spawnRelay(addr);
    Epyx::log::info << "Start Relay at " << addr << Epyx::log::endl;

    // Create nodes
    const int nodeNum = 42;
    Epyx::N2NP::Node* nodes[nodeNum];
    int nodesIndex[nodeNum];
    Epyx::log::info << "Create nodes..." << Epyx::log::endl;
    Ponger testModule;
    Displayer testModule2;
    for (int i = 0; i < nodeNum; i++) {
        nodes[i] = new Epyx::N2NP::Node(addr);
        nodes[i]->addModule("TEST", &testModule);
        nodes[i]->addModule("PONG", &testModule2);
        //nodes[i]->registerRecv(pongType, nodeRecvPong, NULL);
        nodesIndex[i] = epyx.addNode(nodes[i]);
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

    for (int i = 0; i < nodeNum; i++) {
        Epyx::log::info << "Destroy node " << nodeids[i] << Epyx::log::endl;
        epyx.destroyNode(nodesIndex[i]);
    }
    Epyx::log::info << "Destroy relay " << addr << Epyx::log::endl;
    epyx.destroyRelay(2000);
}

/**
 * @brief Setup environment and invoke test_n2np()
 */
int main() {
    Epyx::API epyx;
    try {
        epyx.setNetWorkers(50);
        test_n2np(epyx, Epyx::Address("0.0.0.0:4242"));
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
