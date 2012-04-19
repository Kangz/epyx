#include "packet.h"
#include "../core/common.h"
#include <string.h>

namespace Epyx
{
    namespace N2NP
    {

        Packet::Packet(const PacketType& type, unsigned int size,
            const char *data)
        :from(), to(), type(type), size(size), data(NULL) {
            char *newData = new char[size];
            memcpy(newData, data, size);
            this->data = newData;
        }

        Packet::Packet(const Packet& pkt)
        :from(pkt.from), to(pkt.to), type(pkt.type),
        size(pkt.size) {
            char *newData = new char[pkt.size];
            memcpy(newData, pkt.data, pkt.size);
            data = newData;
        }

        const Packet& Packet::operator=(const Packet& pkt) {
            if (this != &pkt) {
                this->from = pkt.from;
                this->to = pkt.to;
                this->type = pkt.type;
                this->size = pkt.size;
                char *newData = new char[pkt.size];
                memcpy(newData, pkt.data, pkt.size);
                this->data = newData;
            }
            return *this;
        }

        Packet::Packet(const GTTPacket& pkt)
        :size(0), data(NULL) {
            std::map<std::string, std::string>::const_iterator it;

            if (pkt.protocol.compare("N2NP")) {
                log::error << "N2NP::Packet: Invalid GTT protocol "
                    << pkt.protocol << log::endl;
                throw FailException("N2NP::Packet", "Invalid GTT packet");
            }

            for (it = pkt.headers.begin(); it != pkt.headers.end(); it++) {
                if (!it->first.compare("From")) {
                    from = NodeId(it->second);
                } else if (!it->first.compare("To")) {
                    to = NodeId(it->second);
                } else if (!it->first.compare("Version")) {
                    version = it->second;
                } else if (!it->first.compare("Type")) {
                    type = PacketType(it->second);
                } else if (!it->first.compare("ID")) {
                    pktID = String::toInt(it->second);
                } else {
                    log::error << "N2NP::Packet: Unknown header "
                        << it->first << log::endl;
                    throw FailException("N2NP::Packet", "Invalid GTT packet");
                }
            }
            size = pkt.size;
            if (size > 0) {
                char *newData = new char[size];
                memcpy(newData, pkt.body, size);
                data = newData;
            }
        }

        Packet::~Packet() {
            if (data != NULL)
                delete[] data;
        }
    }
}
