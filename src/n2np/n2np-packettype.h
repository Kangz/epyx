/**
 * N2NP Packet Type
 */

#ifndef EPYX_N2NP_PACKET_TYPE_H
#define EPYX_N2NP_PACKET_TYPE_H

#include <ostream>

namespace Epyx
{
    const size_t N2npPacketTypeMaxLen = 50;

    class N2npPacketType
    {
    private:
        char typeStr[N2npPacketTypeMaxLen + 1];

    public:
        N2npPacketType(const char *type);
        friend std::ostream& operator<<(std::ostream& os, const N2npPacketType& type);
        friend std::ostream& operator<<(std::ostream& os, const N2npPacketType *type);

        friend bool operator<(const N2npPacketType& type1, const N2npPacketType& type2);
    };
}

#endif /* EPYX_N2NP_PACKET_TYPE_H */
