#include "framepacket.h"
#include <cstring>

namespace Epyx
{
    namespace webm
    {

        FramePacket::FramePacket(void *data, size_t size, unsigned long timestamp,
            unsigned long duration)
        :data(NULL), size(size), timestamp(timestamp), duration(duration) {
            this->data = new unsigned char[size];
            memcpy(this->data, data, size);
        }

        FramePacket::~FramePacket() {
            delete[] data;
        }
    }
}
