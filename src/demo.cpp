#include "api.h"
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <mutex>
#define LightGreen "\033[01;33m"
#define LightBlue "\033[01;34m"
#define Restore "\033[0m"

using namespace Epyx;

class Demo;
class Displayer :public Epyx::N2NP::Module
{
public:
    Displayer(Demo *demo, const std::string& pseudo_ext);
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const byte_str& data);
private:
    Demo *demo;
    std::string pseudo_ext;
};
class Demo
{
public:
    bool run(Epyx::API& epyx, const Epyx::SockAddress& relayAddr);

    void updateDisplay();

    void receive(const std::string& pseudo, const std::string& msg, bool isMe = false);

private:
    // Clear screen
    void clear();
    // Setup terminal to be char-by-char instead of line-by-line
    void configureTerm();

    std::string msg;
    std::string historique;
    std::mutex histomut;
    std::string pseudo;
    std::shared_ptr<Epyx::N2NP::Node> node;
};

Displayer::Displayer(Demo *demo, const std::string& pseudo_ext)
:demo(demo), pseudo_ext(pseudo_ext) {
    EPYX_ASSERT(demo != NULL);
}

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const byte_str& data) {
    demo->receive(pseudo_ext, bytes2string_c(data));
    demo->updateDisplay();
}

bool Demo::run(Epyx::API& epyx, const Epyx::SockAddress& relayAddr) {
    // Log in
    std::cout << "Entrez votre pseudo : ";
    std::cin >> pseudo;

    // Create DHT node
    node = epyx.spawnN2NPNode(relayAddr);
    std::shared_ptr<DHT::Node> dhtNode = epyx.createDHTNode("DHT", node);

    // Send ping to the relay
    N2NP::NodeId relayNodeId("self", node->getId().getRelay());
    dhtNode->sendPing(DHT::Peer::SPtr(new DHT::Peer(relayNodeId)));

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
    std::shared_ptr<Displayer> displayModule(new Displayer(this, pseudo_ext));
    node->addModule("DISPLAY", displayModule);

    // Let's run !
    unsigned char c;
    configureTerm();
    while (true) {
        updateDisplay();
        read(STDIN_FILENO, &c, 1);
        if (c == '\b' || c == 127) {
            if (msg.length() >= 1)
                msg.erase(msg.length() - 1);
        } else msg.append(1, c);
        if (c == '\n') {
            node->send(remoteNodeid, "DISPLAY", string2bytes(msg));
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
    tios.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &tios);
}

void Demo::receive(const std::string& pseudo, const std::string& msg, bool isMe) {
    std::lock_guard<std::mutex> lock(histomut);
    historique.append(isMe ? LightBlue : LightGreen)
        .append(std::string("<").append(pseudo).append(std::string("> : "))
        .append(msg)).append(Restore);
}

void Demo::updateDisplay() {
    clear();
    std::cout << historique;
    std::cout << "<" << pseudo << "> : " << msg;
    fflush(stdout);
}

int main(int argc, char **argv) {
    Epyx::API epyx;
    try {
        epyx.setNetWorkers(50);
        if (argc < 2) {
            std::cerr << "You need to tell me the relay I am intented to connect." << std::endl;
            return 1;
        }
        Demo demo;
        demo.run(epyx, Epyx::SockAddress(argv[1]));
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
