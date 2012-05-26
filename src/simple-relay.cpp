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
        Epyx::Address relayAddr;

        // Parse options
        int c;
        int nbConn = 50;
        while ((c = getopt(argc, argv, "hn:")) != -1) {
            switch (c) {
                case 'h':
                    std::cout << "Usage: relay [options] address:port" << std::endl
                        << "options:" << std::endl
                        << " -h  Display help" << std::endl
                        << " -n number Set up the number of network workers"
                        << "(which treat packets, " << nbConn << " by default)" << std::endl
                        << "address:port Binded interface" << std::endl;
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

        if (optind < argc -1) {
            std::cerr << "Too many parameters." << std::endl;
            return 1;
        } else if (optind == argc - 1) {
            // One relay was specified
            relayAddr = Epyx::Address(argv[optind]);
            if (relayAddr.empty()) {
                std::cerr << "Empty relay address was given." << std::endl;
                return 1;
            }
        } else {
            // No specified interface : find one
            std::vector<Epyx::Address> addrs = Epyx::Address::getIfaceAdresses(RELAY_DEFAULT_PORT);
            std::vector<Epyx::Address> externAddresses;
            std::vector<Epyx::Address> localAddresses;

            for (std::vector<Epyx::Address>::const_iterator iaddr = addrs.begin();
                iaddr != addrs.end(); iaddr++) {
                // Keep only IPv4
                std::string ipaddr = iaddr->getIp();
                if (ipaddr.empty() || ipaddr.find(':') != std::string::npos)
                    continue;
                // Difference between local and extern
                if (ipaddr.length() > 3 && !ipaddr.substr(0, 4).compare("127."))
                    localAddresses.push_back(*iaddr);
                else
                    externAddresses.push_back(*iaddr);
            }

            if (!externAddresses.empty()) {
                if (externAddresses.size() >= 2) {
                    Epyx::log::info << "More than one extern network interface found. Using first of:"
                        << Epyx::log::endl;
                    for (std::vector<Epyx::Address>::const_iterator iaddr = externAddresses.begin();
                        iaddr != externAddresses.end(); iaddr++) {
                        Epyx::log::info << "* " << *iaddr << Epyx::log::endl;
                    }
                } else {
                    Epyx::log::info << "Found extern network interface " << externAddresses[0]
                        << Epyx::log::endl;
                }
                relayAddr = externAddresses[0];
            } else if (!localAddresses.empty()) {
                Epyx::log::info << "No extern network interface found, using local."
                    << Epyx::log::endl;
                if (localAddresses.size() >= 2) {
                    Epyx::log::info << "More than one local interface found. Using first of:"
                        << Epyx::log::endl;
                    for (std::vector<Epyx::Address>::const_iterator iaddr = localAddresses.begin();
                        iaddr != localAddresses.end(); iaddr++) {
                        Epyx::log::info << "* " << *iaddr << Epyx::log::endl;
                    }
                } else {
                    Epyx::log::info << "Found local network interface " << localAddresses[0]
                        << Epyx::log::endl;
                }
                relayAddr = localAddresses[0];
            } else {
                Epyx::log::info << "No network interface found, exiting."
                    << Epyx::log::endl;
                return 0;
            }
        }

        // Spawn relay
        EPYX_ASSERT(!relayAddr.empty());
        epyx.setNetWorkers(nbConn);
        epyx.spawnRelay(relayAddr, nbConn);
        epyx.waitNet();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
        return 1;
    }
    return 0;
}

