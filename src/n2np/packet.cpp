#include "packet.h"
#include "../core/common.h"
#include "../parser/gttpacket.h"
#include <string.h>

namespace Epyx
{
    namespace N2NP
    {

        Packet::Packet(const std::string method, unsigned int size,
            const char *data)
        :method(method), from(), to(), version(""), pktID(0),
        size(size), data(NULL) {
            if (data != NULL) {
                char *newData = new char[size];
                memcpy(newData, data, size);
                this->data = newData;
            }
        }

        Packet::Packet(const Packet& pkt)
        :method(pkt.method), from(pkt.from), to(pkt.to), version(pkt.version),
        pktID(pkt.pktID), size(pkt.size) {
            char *newData = new char[pkt.size];
            memcpy(newData, pkt.data, pkt.size);
            data = newData;
        }

        const Packet& Packet::operator=(const Packet& pkt) {
            if (this != &pkt) {
                this->from = pkt.from;
                this->to = pkt.to;
                this->size = pkt.size;
                char *newData = new char[pkt.size];
                memcpy(newData, pkt.data, pkt.size);
                this->data = newData;
            }
            return *this;
        }

        Packet::Packet(const GTTPacket& pkt)
        :pktID(0), size(0), data(NULL) {
            std::map<std::string, std::string>::const_iterator it;

            // Check protocol
            if (pkt.protocol.compare("N2NP")) {
                log::error << "N2NP::Packet: Invalid GTT protocol "
                    << pkt.protocol << log::endl;
                throw FailException("N2NP::Packet", "Invalid GTT packet");
            }

            method = pkt.method;

            // Parse headers
            for (it = pkt.headers.begin(); it != pkt.headers.end(); it++) {
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

        std::ostream& operator<<(std::ostream& os, const Packet& pkt) {
            os << "[N2NP " << pkt.from << " > " << pkt.to
                << " method " << pkt.method << " ID " << pkt.pktID
                << " size " << pkt.size << "]";
            return os;
        }

        unsigned long Packet::build(char **newData) const {
            EPYX_ASSERT(newData != NULL);
            // Build a GTT Packet to build
            GTTPacket gttpkt;
            this->fillGTTPacket(gttpkt);
            unsigned long ret = gttpkt.build(newData);
            // Do not free data
            gttpkt.size = 0;
            gttpkt.body = NULL;
            return ret;
        }

        bool Packet::send(Socket& sock) const {
            GTTPacket gttpkt;
            this->fillGTTPacket(gttpkt);

            char *gttdata = NULL;
            unsigned long gttsize = gttpkt.build(&gttdata);
            if (gttdata == NULL) {
                log::error << "N2NP packet: Unable to build packet" << log::endl;
                return false;
            }
            bool result = sock.sendAll(gttdata, gttsize);
            delete[] gttdata;

            // Do not free own data
            gttpkt.size = 0;
            gttpkt.body = NULL;
            return result;
        }

        void Packet::fillGTTPacket(GTTPacket &gttpkt) const {
            gttpkt.protocol = "N2NP";
            gttpkt.method = method;
            gttpkt.headers["From"] = from.toString();
            gttpkt.headers["To"] = to.toString();
            gttpkt.headers["Version"] = (version.empty() ? "0" : version);
            gttpkt.headers["ID"] = String::fromInt(pktID);
            gttpkt.size = size;
            gttpkt.body = data;
        }
    }
}
