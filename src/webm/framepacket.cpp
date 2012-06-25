#include "framepacket.h"
#include "../core/string.h"
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

        FramePacket::FramePacket(GTTPacket& pkt) {
            //Check the packet is not ill-formed
            if (pkt.protocol.compare("WEBM") || pkt.method.compare("FRAME_PACKET")) {
                throw ParserException("WebM::FramePacket", "Invalid packet");

            }
            if (pkt.headers.count("duration") == 0 || pkt.headers.count("timestamp") == 0 || pkt.size == 0) {
                throw ParserException("WebM::FramePacket", "Invalid packet headers");
            }

            //Copy everything from the GTT packet
            timestamp = String::toInt(pkt.headers["timestamp"]);
            duration = String::toInt(pkt.headers["duration"]);
            size = pkt.size;
            data = (unsigned char*) pkt.body;

            //Dereference the content of the GTT packet
            pkt.body = NULL;
            pkt.size = 0;
        }

        unsigned long FramePacket::build(char **newData) const {
            GTTPacket pkt;

            //Fill the GTT packet
            pkt.protocol = "WEBM";
            pkt.method = "FRAME_PACKET";
            pkt.headers["duration"] = String::fromInt(duration);
            pkt.headers["timestamp"] = String::fromInt(timestamp);
            pkt.body = (char*) data;
            pkt.size = size;

            //Return pkt.build()
            unsigned long result = pkt.build(newData);
            pkt.body = NULL;
            pkt.size = 0;
            return result;
        }

        FramePacket::~FramePacket() {
            delete[] data;
        }
    }
}
