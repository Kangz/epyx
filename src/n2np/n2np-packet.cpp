#include "n2np-packet.h"
#include <string.h>

namespace Epyx
{
    N2npPacket::N2npPacket(const N2npPacketType type_, unsigned int size_,
                           const char *data_)
        :from(), to(), type(type_), size(size_), data(data_)
        
    {
    }

    N2npPacket::N2npPacket(const N2npPacket& pkt)
        :from(pkt.from), to(pkt.to), type(pkt.type),
        size(pkt.size)
    {
        char *data_ = new char[pkt.size];
        memcpy(data_, pkt.data, pkt.size);
        this->data = data_;
    }

    const N2npPacket& N2npPacket::operator=(const N2npPacket& pkt)
    {
        this->from = pkt.from;
        this->to = pkt.to;
        this->type = pkt.type;
        this->size = pkt.size;
        char *data_ = new char[pkt.size];
        memcpy(data_, pkt.data, pkt.size);
        this->data = data_;
        return *this;
    }
}
