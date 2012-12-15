#include "api.h"
#include "core/input.h"
#include "natpunching/openconnection.h"

bool test_natpunching(Epyx::API& epyx, const Epyx::SockAddress& relayAddr,
    const Epyx::N2NP::NodeId& remoteNodeId) {
    EPYX_VERIFY(!relayAddr.empty());
    EPYX_VERIFY(!remoteNodeId.empty());

    // Create DHT node
    std::shared_ptr<Epyx::N2NP::Node> node = epyx.spawnN2NPNode(relayAddr);
    std::shared_ptr<Epyx::DHT::Node> dhtNode = epyx.createDHTNode("DHT", node);

    // Send ping to the relay
    Epyx::N2NP::NodeId relayNodeId("self", node->getId().getRelay());
    dhtNode->sendPing(Epyx::DHT::Peer::SPtr(new Epyx::DHT::Peer(relayNodeId)));

    Epyx::log::info << "My node is " << node->getId() << Epyx::log::endl;
    Epyx::log::info << "DHT " << (*(dhtNode->getConnectionInfo())) << Epyx::log::endl;

    // Test to open a connection being server
    Epyx::log::info << "Try to open connection with " << remoteNodeId << Epyx::log::endl;
    std::shared_ptr<Epyx::DirectConnection::OpenConnection> oconn(
        new Epyx::DirectConnection::OpenConnection(node, remoteNodeId, false));

    // Wait for interrupt
    Epyx::Input::waitForInt();
    return true;
}

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        // Parse options
        int c;
        int nbWorkers = 50;
        while ((c = getopt(argc, argv, "hw:")) != -1) {
            switch (c) {
                case 'h':
                    std::cout << "Usage: test-natpunching [options] address:port nodeId" << std::endl
                        << "options:" << std::endl
                        << " -h Display help" << std::endl
                        << " -w number Set the number of network workers"
                        << " (which treat packets, " << nbWorkers << " by default)" << std::endl
                        << "address:port Bound interface" << std::endl
                        << "nodeId ID of the node to establish a contact with" << std::endl;
                    return 0;
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

        if (optind != argc - 2) {
            std::cerr << "Wrong number of parameters: need 2, got "
                << (argc - optind) << ". Use -h to get help." << std::endl;
            return 1;
        }

        const Epyx::SockAddress relayAddr(argv[optind]);
        const Epyx::N2NP::NodeId remoteNodeId(argv[optind + 1]);
        epyx.setNetWorkers(nbWorkers);
        test_natpunching(epyx, relayAddr, remoteNodeId);
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
