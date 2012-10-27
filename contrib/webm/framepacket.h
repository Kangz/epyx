/**
 * @file webm/framepacket.h
 */

#ifndef EPYX_WEBM_FRAMEPACKET_H
#define EPYX_WEBM_FRAMEPACKET_H

#include <boost/noncopyable.hpp>
#include <stddef.h>
#include "../../src/core/byte.h"
#include "../../src/parser/gttpacket.h"

namespace Epyx
{
    namespace webm
    {
        class FramePacket : private boost::noncopyable
        {
        public:
            FramePacket(const byte_str& data, unsigned long timestamp, unsigned long duration);
            FramePacket(GTTPacket& pkt);

            byte_str build() const;

            byte_str data;

            // Timestamp in miliseconds
            unsigned long timestamp;
            unsigned long duration;
        };
    }
}

#endif /* EPYX_WEBM_FRAMEPACKET_H */

