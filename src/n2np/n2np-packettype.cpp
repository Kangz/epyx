#include "n2np-packettype.h"
#include <string.h>

namespace Epyx
{
    N2npPacketType::N2npPacketType(const char *type)
    {
        strncpy(this->typeStr, type, N2npPacketTypeMaxLen);
        this->typeStr[N2npPacketTypeMaxLen] = 0;
    }

    std::ostream& operator<<(std::ostream& os, const N2npPacketType& type)
    {
        return os << type.typeStr;
    }

    std::ostream& operator<<(std::ostream& os, const N2npPacketType *type)
    {
        if (!type)
            return os << "(null)";
        else
            return os << (*type);
    }

    int operator<(const N2npPacketType& type1, const N2npPacketType& type2)
    {
        return strncmp(type1.typeStr, type2.typeStr, N2npPacketTypeMaxLen);
    }
}
