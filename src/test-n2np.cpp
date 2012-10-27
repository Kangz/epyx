/**
 * @file test-n2np.cpp
 * @brief This program tests Node to Node Protocol
 */

#include "api.h"
#include <cstring>
class Ponger :public Epyx::N2NP::Module
{
public:
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const Epyx::byte_str& data);
};

void Ponger::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const Epyx::byte_str& data) {
    if (data.empty()) {
        // If no data was received, pong with null
        Epyx::log::debug << "[Node " << node.getId() << "] Recv from " << from
            << ": (null)" << Epyx::log::endl;
        node.send(from, "PONG", NULL, 0);
        return;
    }
    Epyx::log::debug << "[Node " << node.getId() << "] Recv from " << from << ": `"
        << bytes2string_c(data) << "'" << Epyx::log::endl;
    // Send a pong with the same data
    // .. or not
    std::string msg = bytes2string_c(data);
    if (!strcasecmp(msg.c_str(), "o<"))
        msg = "PAN !";
    else if (!strcasecmp(msg.c_str(), "Question?"))
        msg = "The answer is 42.";

    node.send(from, "PONG", string2bytes_c(msg));
}
class Displayer :public Epyx::N2NP::Module
{
public:
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const Epyx::byte_str& data);
};

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const Epyx::byte_str& data) {
    Epyx::log::debug << "[Node " << node.getId() << "] Pong from " << from << ": `"
        << bytes2string_c(data) << "'" << Epyx::log::endl;
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
    std::shared_ptr<Epyx::N2NP::Node> firstNode;
    Epyx::N2NP::NodeId nodeids[nodeNum];
    Epyx::log::info << "Create " << nodeNum << " nodes..." << Epyx::log::endl;
    std::shared_ptr<Ponger> testModule(new Ponger);
    std::shared_ptr<Displayer> testModule2(new Displayer);
    for (int i = 0; i < nodeNum; i++) {
        std::shared_ptr<Epyx::N2NP::Node> node = epyx.spawnN2NPNode(addr);
        if (!firstNode)
            firstNode = node;
        node->addModule("TEST", testModule);
        node->addModule("PONG", testModule2);
        nodeids[i] = node->getId();
    }

    // Let's run a Command-Line Interface !
    int nodeIndex;
    std::string msg;

    Epyx::log::debug << "[Cli:motd] Command-line interface" << Epyx::log::endl;
    Epyx::log::debug << "[Cli:motd] You are on " << firstNode->getId() << Epyx::log::endl;
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

            // Send !
            const Epyx::N2NP::NodeId& nodeid = nodeids[nodeIndex - 1];
            Epyx::log::debug << "[Cli] Sending " << msg << " to " << nodeid << Epyx::log::endl;
            firstNode->send(nodeid, "TEST", string2bytes_c(msg));
        }
    }
}

/**
 * @brief Setup environment and invoke test_n2np()
 */
int main() {
    Epyx::API epyx;
    try {
        epyx.setNetWorkers(50);
        test_n2np(epyx, Epyx::SockAddress("127.0.0.1:4242"));
        Epyx::log::info << "Destroy everything !" << Epyx::log::endl;
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
