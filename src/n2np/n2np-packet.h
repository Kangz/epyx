/**
 * N2NP Packet
 */

#ifndef EPYX_N2NP_PACKET_H
#define EPYX_N2NP_PACKET_H

#include "n2np-nodeid.h"
#include "n2np-packettype.h"

namespace Epyx
{
    class N2npPacket
    {
    public:
        N2npNodeId from;
        N2npNodeId to;
        // TODO: Version
        N2npPacketType type;
        unsigned int size;
        const char *data;

        N2npPacket(const N2npPacketType type_, unsigned int size_,
                   const char *data_);

        // Deep copy
        N2npPacket(const N2npPacket& pkt);
        const N2npPacket& operator=(const N2npPacket& pkt);
    };
}

#endif /* EPYX_N2NP_PACKET_H */
