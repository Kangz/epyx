#include "packettype.h"

namespace Epyx
{
    namespace N2NP
    {
        PacketType::PacketType()
        :type() {
        }

        PacketType::PacketType(const std::string& type)
        :type(type) {
        }

        std::ostream& operator<<(std::ostream& os, const PacketType& pkttype) {
            return os << pkttype.type;
        }

        bool operator<(const PacketType& pkttype1, const PacketType& pkttype2) {
            return pkttype1.type < pkttype2.type;
        }
    }
}
