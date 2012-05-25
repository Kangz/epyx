/**
 * @file simple-relay.cpp
 * @brief Simple relay program
 */

#include "api.h"
#include <unistd.h>

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        // Parse options
        int c;
        int nbConn = 50;
        while ((c = getopt(argc, argv, "hn:")) != -1) {
            switch (c) {
                case 'h':
                    std::cout << "Usage: relay [options] address:port [address:port ...]" << std::endl
                        << "options:" << std::endl
                        << " -h  Display help" << std::endl
                        << " -n number Set up the number of network workers"
                        << "(which treat packets, " << nbConn << " by default)" << std::endl
                        << "address:port Binded interface(s)" << std::endl;
                    return 0;
                case 'n':
                    nbConn = Epyx::String::toInt(optarg);
                    break;
                case '?':
                    if (optopt == 'n')
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
        if (optind >= argc) {
            Epyx::log::info << "No relay specified" << Epyx::log::endl;
        }

        epyx.setNetWorkers(nbConn);

        // Spwan relays
        for (int index = optind; index < argc; index++) {
            const Epyx::Address addr(argv[index]);
            epyx.spawnRelay(addr, nbConn);
        }
        epyx.waitNet();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}

