/**
 * @file webm/framepacket.h
 */

#ifndef EPYX_WEBM_FRAMEPACKET_H
#define EPYX_WEBM_FRAMEPACKET_H

#include <stddef.h>
#include "../parser/gttpacket.h"

namespace Epyx
{
    namespace webm
    {
        class FramePacket
        {
        public:
            FramePacket(void *data, size_t size, unsigned long timestamp, unsigned long duration);
            FramePacket(GTTPacket& pkt);
            ~FramePacket();

            unsigned long build(char **newData) const;

            unsigned char *data;
            size_t size;

            // Timestamp in miliseconds
            unsigned long timestamp;
            unsigned long duration;

        private:
            FramePacket(const FramePacket&);
            const FramePacket& operator=(const FramePacket&);
        };
    }
}

#endif /* EPYX_WEBM_FRAMEPACKET_H */

