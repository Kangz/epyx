#include "api.h"
#include "n2np/relay.h"
#include "n2np/node.h"
#include "webm/videodev.h"
#include "webm/vpxdecoder.h"
#include "webm/vpxencoder.h"
#include "webm/videoframe.h"
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

class VideoDisplayer :public Epyx::N2NP::Module
{
public:
    VideoDisplayer(Demo *demo);
    virtual void fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size);
private:
    Demo *demo;
};

class SenderAndUIThread :public Thread
{
public:
    SenderAndUIThread(Demo* demo);
    virtual void run();

private:
    Demo* demo;
};

class Demo
{
public:
    Demo(Epyx::N2NP::Node *node);
    bool run();

    void updateDisplay();

    void receive(const std::string& pseudo, const std::string& msg, bool isMe = false);

    void stop();

    void newFrame(const webm::FramePacket& fpkt);

    webm::VideoFrame vframe;
    Epyx::N2NP::Node *node;
    N2NP::NodeId remoteNodeid;

private:
    // Clear screen
    void clear();
    // Setup terminal to be char-by-char instead of line-by-line
    void configureTerm();

    std::string msg;
    webm::VpxDecoder decoder;
    std::string historique;
    Mutex histomut;
    std::string pseudo;
    bool vframe_inited;
    bool running;
};

Displayer::Displayer(Demo *demo, const std::string& pseudo_ext)
:demo(demo), pseudo_ext(pseudo_ext) {
    EPYX_ASSERT(demo != NULL);
}

void Displayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    demo->receive(pseudo_ext, data);
    demo->updateDisplay();
}

VideoDisplayer::VideoDisplayer(Demo* demo)
:demo(demo) {
}

void VideoDisplayer::fromN2NP(Epyx::N2NP::Node& node, Epyx::N2NP::NodeId from, const char* data, unsigned int size) {
    Epyx::GTTParser parser;
    parser.eat(data, size);
    GTTPacket* gttpkt = parser.getPacket();
    webm::FramePacket fpkt(*gttpkt);
    demo->newFrame(fpkt);
    delete gttpkt;
}


SenderAndUIThread::SenderAndUIThread(Demo* demo)
:Thread("UI"), demo(demo) {
}

void SenderAndUIThread::run() {
    webm::VideoDev vdev(640, 480, 30);
    webm::VpxEncoder encoder(640, 480, 400);

    vpx_image_t rawImage;
    vpx_img_alloc(&rawImage, IMG_FMT_YV12, 640, 480, 1);

    bool vdev_started = vdev.start_capture();
    while(! demo->vframe.checkSDLQuitAndEvents()) {
        if(vdev_started && vdev.get_frame(&rawImage)) {
            struct timeval tv;
            int gettimeofday_status = gettimeofday(&tv, NULL);
            EPYX_ASSERT(gettimeofday_status == 0);
            long int utime = tv.tv_sec * 1000 + tv.tv_usec;

            encoder.encode(rawImage, utime, 0);
            webm::FramePacket* fpkt;
            while ((fpkt = encoder.getPacket()) != NULL) {
                char* netdata;
                unsigned long netsize = fpkt->build(&netdata);

                demo->node->send(demo->remoteNodeid, "VIDEO", netdata, netsize);
            }
        }
        usleep(1000);
    }
    demo->stop();
}

Demo::Demo(Epyx::N2NP::Node *node)
:vframe(640, 480, "Epyx Chat"), node(node), vframe_inited(false) {
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

    VideoDisplayer videoModule(this);
    node->addModule("VIDEO", &videoModule);

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
    remoteNodeid = N2NP::NodeId(remoteNodeidStr);

    log::debug << pseudo_ext << " est dans " << remoteNodeid << log::endl;

    // Add displayer module to my N2NP node
    Displayer displayModule(this, pseudo_ext);
    node->addModule("DISPLAY", &displayModule);

    // Let's run !
    running = true;
    vframe.init();

    vframe_inited = true;

    SenderAndUIThread ui_thread(this);
    ui_thread.start();

    unsigned char c;
    configureTerm();
    while (running) {
        updateDisplay();
        read(STDIN_FILENO, &c, 1);
        msg.append(1, c);
        if (c == '\n') {
            node->send(remoteNodeid, "DISPLAY", msg.c_str(), msg.length() + 1);
            receive(pseudo, msg, true);
            msg.assign("");
        }
    }

    return true;
}

void Demo::stop() {
    running = false;
}

void Demo::newFrame(const webm::FramePacket& fpkt) {
    decoder.decode(fpkt);
    vpx_image_t *img = decoder.getFrame();
    if (img != NULL && vframe_inited) {
        vframe.showFrame(img);
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
