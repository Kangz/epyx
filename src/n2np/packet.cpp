#include "packet.h"
#include "../core/common.h"
#include "../parser/gttpacket.h"
#include <string.h>

namespace Epyx
{
    namespace N2NP
    {

        Packet::Packet(const std::string& method, const byte_str& data)
        :method(method), from(), to(), version(""), pktID(0), data(data) {
        }

        Packet::Packet(const GTTPacket& pkt)
        :pktID(0), data() {
            // Check protocol
            if (pkt.protocol.compare("N2NP")) {
                log::error << "N2NP::Packet: Invalid GTT protocol "
                    << pkt.protocol << log::endl;
                throw FailException("N2NP::Packet", "Invalid GTT packet");
            }

            method = pkt.method;

            // Parse headers
            for (auto it = pkt.headers.begin(); it != pkt.headers.end(); it++) {
                if (!it->first.compare("From")) {
                    from = NodeId(it->second);
                } else if (!it->first.compare("To")) {
                    to = NodeId(it->second);
                } else if (!it->first.compare("Version")) {
                    version = it->second;
                } else if (!it->first.compare("ID")) {
                    pktID = String::toInt(it->second);
                } else {
                    log::error << "N2NP::Packet: Unknown header "
                        << it->first << log::endl;
                    throw MinorException("N2NP::Packet", "Invalid GTT header");
                }
            }
            data = pkt.body;
        }

        std::ostream& operator<<(std::ostream& os, const Packet& pkt) {
            os << "[N2NP " << pkt.from << " > " << pkt.to
                << " method " << pkt.method << " ID " << pkt.pktID
                << " size " << pkt.data.size() << "]";
            return os;
        }

        byte_str Packet::build() const {
            // Build a GTT Packet to build
            GTTPacket gttpkt;
            gttpkt.protocol = "N2NP";
            gttpkt.method = method;
            gttpkt.headers["From"] = from.toString();
            gttpkt.headers["To"] = to.toString();
            gttpkt.headers["Version"] = (version.empty() ? "0" : version);
            gttpkt.headers["ID"] = String::fromInt(pktID);
            gttpkt.body = data;
            return gttpkt.build();
        }
    }
}
