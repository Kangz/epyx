/**
 * @file test-n2np.cpp
 * @brief This program tests Node to Node Protocol
 */

#include "core/common.h"
#include "n2np/relay.h"

/**
 * @brief Test local N2NP implementation
 */
void test_n2np() {
    // Create Net Select
    Epyx::NetSelect selectThread(20, "SelectWorker");
    selectThread.setName("NetSelect");
    selectThread.start();

    // Create relay
    Epyx::Address addr("127.0.0.1:4242");
    Epyx::N2NP::Relay relay(addr);
    selectThread.add(new Epyx::N2NP::RelayServer(new Epyx::TCPServer(addr.getPort(), 50), &relay));
    Epyx::log::info << "Start Relay " << relay.getId() << Epyx::log::endl;

    // Wait select
    selectThread.wait();
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
