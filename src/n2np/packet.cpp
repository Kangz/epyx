#include "packet.h"
#include <string.h>

namespace Epyx
{
    namespace N2NP
    {

        Packet::Packet(const PacketType& type, unsigned int size,
            const char *data)
        :from(), to(), type(type), size(size), data(data) {
        }

        Packet::Packet(const Packet& pkt)
        :from(pkt.from), to(pkt.to), type(pkt.type),
        size(pkt.size) {
            char *newData = new char[pkt.size];
            memcpy(newData, pkt.data, pkt.size);
            this->data = newData;
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
    }
}
