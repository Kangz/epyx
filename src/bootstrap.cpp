/**
 * @file bootstrap.cpp
 * @brief Boot-strap Epyx network
 */

#include "api.h"
#include <unistd.h>
#include "core/input.h"

/**
 * @brief Default port for relay
 */
#define RELAY_DEFAULT_PORT 4242

typedef std::shared_ptr<Epyx::N2NP::Node> N2NP_Node_ptr;
typedef std::shared_ptr<Epyx::DHT::Node> DHT_Node_ptr;

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        Epyx::SockAddress relayAddr;

        // Parse options
        int c;
        int nbWorkers = 50;
        int nbNodes = 10;
        while ((c = getopt(argc, argv, "hn:w:")) != -1) {
            switch (c) {
                case 'h':
                    std::cout << "Usage: bootstrap [options] address:port" << std::endl
                        << "options:" << std::endl
                        << " -h Display help" << std::endl
                        << " -w number Set the number of network workers"
                        << " (which treat packets, " << nbWorkers << " by default)" << std::endl
                        << " -n number Set the network size"
                        << " (number of nodes to be created, " << nbNodes << " by default)" << std::endl
                        << "address:port Bound interface" << std::endl;
                    return 0;
                case 'n':
                    nbNodes = Epyx::String::toInt(optarg);
                    break;
                case 'w':
                    nbWorkers = Epyx::String::toInt(optarg);
                    break;
                case '?':
                    if (optopt == 'n' || optopt == 'w')
                        std::cerr << "Option -" << optopt << " requires an argument." << std::endl;
                    else if (isprint(optopt))
                        std::cerr << "Unknown option `-" << optopt << "'." << std::endl;
                    else
                        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                    return 1;
                default:
                    throw Epyx::FailException("Main", "Invalid case");
            }
        }

        if (optind < argc - 1) {
            std::cerr << "Too many parameters." << std::endl;
            return 1;
        } else if (optind == argc - 1) {
            // One relay was specified
            relayAddr = Epyx::SockAddress(argv[optind]);
            if (relayAddr.empty()) {
                std::cerr << "Empty relay address was given." << std::endl;
                return 1;
            }
        } else {
            // No specified interface : find one
            Epyx::NetIf iface = Epyx::NetIf::pickExternalInterface(4, true);
            relayAddr = Epyx::SockAddress(iface.getAddress(), RELAY_DEFAULT_PORT);
        }

        // Spawn relay
        EPYX_VERIFY(!relayAddr.empty());
        if (nbWorkers < 1) nbWorkers = 1;
        epyx.setNetWorkers(nbWorkers);
        epyx.spawnRelay(relayAddr, nbWorkers);

        if (nbNodes >= 1) {
            // Allocate things
            std::unique_ptr < N2NP_Node_ptr[] > nodes(new N2NP_Node_ptr[nbNodes]);
            std::unique_ptr < DHT_Node_ptr[] > dhtNodes(new DHT_Node_ptr[nbNodes]);

            // Create first node
            nodes[0] = epyx.spawnN2NPNode(relayAddr);
            dhtNodes[0] = epyx.createDHTNode("DHT-0", nodes[0], true);

            // Create other nodes
            for (int i = 1; i < nbNodes; i++) {
                nodes[i] = epyx.spawnN2NPNode(relayAddr);
                std::ostringstream o;
                o << "DHT-";
                o << i;
                dhtNodes[i] = epyx.createDHTNode(o.str(), nodes[i]);

                // Send pings
                for (int j = 0; j < i; j++) {
                    dhtNodes[i]->sendPing(dhtNodes[j]->getConnectionInfo());
                }
            }

            // View information
            Epyx::log::info << "Created " << nbNodes << " nodes:" << Epyx::log::endl;
            for (int i = 0; i < nbNodes; i++) {
                Epyx::log::info << " [" << i << "] " << (*(dhtNodes[i]->getConnectionInfo())) << Epyx::log::endl;
            }
        }

        // Wait for interrupt
        Epyx::Input::waitForInt();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
        return 1;
    }
    return 0;
}
