/**
 * @file simple-relay.cpp
 * @brief Simple relay program
 */

#include "api.h"
#include <unistd.h>

/**
 * @brief Default port for relay
 */
#define RELAY_DEFAULT_PORT 4242

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        Epyx::SockAddress relayAddr;

        // Parse options
        int c;
        int nbWorkers = 50;
        while ((c = getopt(argc, argv, "hw:")) != -1) {
            switch (c) {
                case 'h':
                    std::cout << "Usage: relay [options] address:port" << std::endl
                        << "options:" << std::endl
                        << " -h   Display help" << std::endl
                        << " -w number Set the number of network workers"
                        << " (which treat packets, " << nbWorkers << " by default)" << std::endl
                        << "address:port Binded interface" << std::endl;
                    return 0;
                case 'w':
                    nbWorkers = Epyx::String::toInt(optarg);
                    break;
                case '?':
                    if (optopt == 'w')
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
        epyx.setNetWorkers(nbWorkers);
        epyx.spawnRelay(relayAddr, nbWorkers);
        epyx.waitNet();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
        return 1;
    }
    return 0;
}
