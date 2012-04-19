#include "core/common.h"
#include "parser/gttparser.h"
#include "n2np/packet.h"

// Maximum packet length
static const int MAXLENGTH = 10000;

bool test_parser() {
    // Build a N2NP packet
    Epyx::N2NP::PacketType testtype("Test");
    std::string message("Hello World. Let me introduce you to Epyx.");
    Epyx::N2NP::Packet pkt(testtype, message.length(), message.c_str());
    pkt.from = Epyx::N2NP::NodeId("Epyx@129.104.13.37:42");
    pkt.to = Epyx::N2NP::NodeId("World@");
    pkt.method = "PING";
    pkt.version = "1";
    pkt.pktID = 2010;

    // Raw packet
    Epyx::log::info << "Building packet " << pkt << Epyx::log::endl;
    char *data = NULL;
    unsigned long size = pkt.build(&data);
    Epyx::log::info << "Raw packet:\n" << std::string(data, size) << Epyx::log::endl;

    // Parse
    Epyx::GTTParser parser;
    Epyx::GTTPacket *gttpkt = NULL;
    parser.eat(data, size);
    gttpkt = parser.getPacket();
    delete[] data;
    data = NULL;
    if (gttpkt == NULL) {
        Epyx::log::info << "Incomplete raw packet :(" << Epyx::log::endl;
        return false;
    }
    Epyx::log::info << "Parsed GTT packet:\n" << *gttpkt << Epyx::log::endl;
    // Create N2NP packet from GTT
    Epyx::N2NP::Packet n2nppkt(*gttpkt);
    delete gttpkt;
    gttpkt = NULL;
    Epyx::log::info << "Parsed N2NP packet: " << n2nppkt << Epyx::log::endl;
    return true;
}

int main() {
    try {
        Epyx::Thread::init();
        Epyx::log::init(Epyx::log::CONSOLE, "");
        test_parser();
    } catch (Epyx::Exception e) {
        std::cerr << e << "\n";
    }
    Epyx::log::flushAndQuit();
    return 0;


/*
    std::string msgName;
    char message[MAXLENGTH];
    Epyx::Thread::init();
    Epyx::log::init(Epyx::log::CONSOLE, "");
    try {
        Epyx::GTTParser myParser;
        // TODO: Use argc/argv to get msg.txt path
        std::ifstream msgList("msg.txt");
        if (!msgList) {
            Epyx::log::fatal << "Please provide `msg.txt' to indicate all filenames of messages"
                << Epyx::log::endl;
            throw Epyx::FailException("testParser", "No message list found");
        }

        while (!msgList.eof()) {
            getline(msgList, msgName);
            // Remove space characters
            Epyx::String::trim(msgName);
            if (msgName.empty())
                continue;
            Epyx::log::debug << "Read message `" << msgName << "'" << Epyx::log::endl;
            std::ifstream msgInput(msgName.c_str(), std::ios_base::in | std::ios_base::binary);
            if (!msgInput) {
                Epyx::log::fatal << "Unable to read `" << msgName << "'" << Epyx::log::endl;
                throw Epyx::FailException("testParser", "Message error");
            }

            while (!msgInput.eof()) {
                msgInput.read(message, sizeof message);
                int size = msgInput.gcount();
                if (size > 0)
                    myParser.eat(message, size);
                else {
                    // A reading probleme MUST be because end of file happens
                    EPYX_ASSERT(msgInput.eof());
                }
                Epyx::GTTPacket *pac;
                while ((pac = myParser.getPacket()) != NULL) {
                    Epyx::N2NP::Packet n2npPkt(*pac);
                    Epyx::log::info << "here comes one packet:\n"
                        << n2npPkt << Epyx::log::endl;
                    char *rebuilt;
                    unsigned long size = n2npPkt.build(&rebuilt);
                    Epyx::log::info << "Rebuilt packet:\n"
                        << std::string(rebuilt, size) << Epyx::log::endl;
                    delete pac;
                    pac = NULL;
                }
            }
            msgInput.close();
        }
        msgList.close();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    Epyx::log::flushAndQuit();
    return 0;
*/
}
