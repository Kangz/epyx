#include "api.h"

// Maximum packet length
static const int MAXLENGTH = 10000;

bool test_parser() {
    // Build a N2NP packet
    std::string testmethod("Test");
    std::string message("Hello World. Let me introduce you to Epyx.");
    Epyx::N2NP::Packet pkt(testmethod, string2bytes_c(message));
    pkt.from = Epyx::N2NP::NodeId("Epyx@129.104.13.37:42");
    pkt.to = Epyx::N2NP::NodeId("World@");
    pkt.method = "PING";
    pkt.version = "1";
    pkt.pktID = 2010;

    // Raw packet
    Epyx::log::info << "Building packet " << pkt << Epyx::log::endl;
    Epyx::byte_str data = pkt.build();
    Epyx::log::info << "Raw packet:\n" << bytes2string_c(data) << Epyx::log::endl;

    // Parse
    Epyx::GTTParser parser;
    std::unique_ptr<Epyx::GTTPacket> gttpkt;
    parser.eat(data);
    gttpkt = parser.getPacket();
    data.clear();
    if (!gttpkt) {
        Epyx::log::info << "Incomplete raw packet :(" << Epyx::log::endl;
        return false;
    }
    Epyx::log::info << "Parsed GTT packet:\n" << *gttpkt << Epyx::log::endl;
    // Create N2NP packet from GTT
    Epyx::N2NP::Packet n2nppkt(*gttpkt);
    Epyx::log::info << "Parsed N2NP packet: " << n2nppkt << Epyx::log::endl;
    return true;
}

int main() {
    Epyx::API epyx;
    try {
        test_parser();
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}
