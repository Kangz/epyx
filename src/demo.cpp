#include "api.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include "dht/node.h"
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#define LightGreen "\033[01;33m"
#define LightBlue "\033[01;34m"
#define Restore "\033[0m"

using namespace Epyx;

class Demo;
class Displayer :public Epyx::N2NP::Module
{
public:
    Displayer(Demo *demo, const std::string& pseudo_ext);
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
private:
    Demo *demo;
    std::string pseudo_ext;
};
class Demo
{
public:
    Demo(Epyx::N2NP::Node *node);
    bool run();

    void updateDisplay();

    void receive(const std::string& pseudo, const std::string& msg, bool isMe = false);

private:
    // Clear screen
    void clear();
    // Setup terminal to be char-by-char instead of line-by-line
    void configureTerm();

    std::string msg;
    std::string historique;
    Mutex histomut;
    std::string pseudo;
    Epyx::N2NP::Node *node;
};

Displayer::Displayer(Demo *demo, const std::string& pseudo_ext)
:demo(demo), pseudo_ext(pseudo_ext) {
    EPYX_ASSERT(demo != NULL);
}

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    demo->receive(pseudo_ext, data);
    demo->updateDisplay();
}

Demo::Demo(Epyx::N2NP::Node *node)
:node(node) {
}

bool Demo::run() {
    EPYX_ASSERT(node != NULL);
    // Log in
    std::cout << "Entrez votre pseudo : ";
    std::cin >> pseudo;

    // Create DHT node
    DHT::Id id(DHT::Id::INIT_RANDOM);
    DHT::Node *dhtNode = new DHT::Node(id, *node, "DHT");
    node->addModule("DHT", dhtNode);

    // Send ping ro the relay
    N2NP::NodeId relayNodeId("self", node->getId().getRelay());
    DHT::Peer relayPeer(relayNodeId);
    dhtNode->sendPing(relayPeer);

    // Wait the ping to be proceeded
    sleep(1);

    if (!dhtNode->setValueSync(Demo::pseudo, node->getId().toString())) {
        std::cerr << "Impossible d'enregistrer une valeur !" << std::endl;
        return false;
    }

    // Ask remote ID
    std::string pseudo_ext;
    std::cout << "Entrez le pseudo que vous voulez contacter : ";
    std::cin >> pseudo_ext;

    // Retreive remote node ID
    std::string remoteNodeidStr;
    while (!dhtNode->getValueSync(pseudo_ext, remoteNodeidStr)) {
        std::cout << "En attente...\n";
        sleep(1);
    }
    Epyx::N2NP::NodeId remoteNodeid(remoteNodeidStr);

    log::debug << pseudo_ext << " est dans " << remoteNodeid << log::endl;

    // Add displayer module to my N2NP node
    Displayer displayModule(this, pseudo_ext);
    node->addModule("DISPLAY", &displayModule);

    // Let's run !
    unsigned char c;
    while (true) {
        updateDisplay();
        read(STDIN_FILENO, &c, 1);
        msg.append(1, c);
        if (c == '\n') {
            node->send(remoteNodeid, "DISPLAY", msg.c_str(), msg.length() + 1);
            receive(pseudo, msg, true);
            msg.assign("");
        }
    }
}

void Demo::clear() {
    system("clear");
}

void Demo::configureTerm() {
    struct termios tios;
    tcgetattr(STDIN_FILENO, &tios);
    //tcflag_t old_c_lflag = tios.c_lflag;
    tios.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);
}

void Demo::receive(const std::string& pseudo, const std::string& msg, bool isMe) {
    histomut.lock();
    historique.append(isMe ? LightBlue : LightGreen)
        .append(std::string("<").append(pseudo).append(std::string("> : "))
        .append(msg)).append(Restore);
    histomut.unlock();
}

void Demo::updateDisplay() {
    // Display
    clear();
    std::cout << historique;
    std::cout << "<" << pseudo << "> : " << msg;
    fflush(stdout);

}

int main(int argc, char **argv) {
    try {
        Epyx::API epyx;
        epyx.setNetWorkers(50);

        // Create my node
        if (argc < 2) {
            std::cerr << "You need to tell me the relay I am intented to connect." << std::endl;
            return 1;
        }
        Epyx::Address relayAddr(argv[1]);
        Epyx::N2NP::Node *node = new Epyx::N2NP::Node(relayAddr);
        epyx.addNode(node);
        if (!node->waitReady(5000)) {
            Epyx::log::error << "Initialisation of node took too much time"
                << Epyx::log::endl;
            epyx.destroyAllNodes();
            epyx.destroyRelay(2000);
            return 1;
        }

        // Now start demo
        Demo demo(node);
        demo.run();
    } catch (Epyx::Exception e) {
        std::cout << e << std::endl;
    }
    return 0;
}
