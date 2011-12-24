/**
 * This program tests Node to Node Protocol
 */

#include <iostream>
#include <sstream>
#include "localn2np/local-node.h"
#include "core/exception.h"

/**
 * Command-line interface to send packets
 */
void test_command(Epyx::LocalNode *node, const Epyx::Address& addr)
{
    unsigned int id;
    std::string msg;
    Epyx::N2npPacketType type("test");

    // Waiting for other threads
    usleep(10);

    std::cout << "[Cli:motd] Command-line interface\n";
    std::cout << "[Cli:motd] You are on " << node << "\n";
    std::cout << "[Cli:motd] 2 Hello sends `Hello' to node 2\n";
    std::cout << "[Cli:motd] 0 quits.\n";

    while (true) {
        std::cin >> id;
        if (id <= 0) {
            std::cout << "[Cli] Bye :)\n";
            return;
        } else {
            std::cin >> msg;
            std::cout << "[Cli] Sending " << msg << " to " << id << "\n";

            // Convert id to a node name
            std::ostringstream idStream;
            idStream << id;
            Epyx::N2npNodeId nodeTo(idStream.str().c_str(), addr);

            // Build a new packet
            Epyx::N2npPacket pkt(type, msg.size() + 1, msg.c_str());

            // Send !
            node->send(nodeTo, pkt);
        }
    }
}

/**
 * Receive callback for every node
 */
bool nodeRecv(Epyx::LocalNode& node, const Epyx::N2npPacket& pkt, void* arg_)
{
    std::cout << "[Node " << node << "] Recv from " << pkt.from << ": `"
        << pkt.data << "'\n";
    return true;
}

int main()
{
    const int nodeNum = 42;
    Epyx::LocalNode* nodes[nodeNum];
    Epyx::LocalRelay *relay = NULL;
    try {
        Epyx::Address addr("L0C4L", 0, 0);
        Epyx::N2npPacketType type("test");

        // Create a relay
        relay = new Epyx::LocalRelay(addr);
        relay->run();

        // Create nodes
        for (int i = 0; i < nodeNum; i++) {
            nodes[i] = new Epyx::LocalNode();
            nodes[i]->attach(relay);
            nodes[i]->registerRecv(type, nodeRecv, NULL);
            nodes[i]->run();
        }

        test_command(nodes[0], addr);
    } catch (Epyx::Exception e) {
        std::cerr << e;
    }
    if (relay)
        delete relay;
    for (int i = 0; i < nodeNum; i++) {
        if (nodes[i])
            delete nodes[i];
    }

    return 0;
}
