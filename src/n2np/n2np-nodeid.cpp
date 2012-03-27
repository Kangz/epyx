#include "n2np-nodeid.h"
#include <string.h>

namespace Epyx
{
    N2npNodeId::N2npNodeId()
        :relay()
    {
        this->name[0] = 0;
    }

     N2npNodeId::N2npNodeId(const Address relay_)
        :relay(relay_)
    {
        this->name[0] = 0;
    }

   N2npNodeId::N2npNodeId(const char *name_, const Address relay_)
        :relay(relay_)
    {
        strncpy(this->name, name_, N2npNodeNameMaxLen);
        this->name[N2npNodeNameMaxLen] = 0;
    }

    std::ostream& operator<<(std::ostream& os, const N2npNodeId nodeid)
    {
        return os << nodeid.name << '@' << nodeid.relay;
    }

    std::ostream& operator<<(std::ostream& os, const N2npNodeId *nodeid)
    {
        if (!nodeid)
            return os << "(null)";
        else
            return os << (*nodeid);
    }

    bool operator==(const N2npNodeId& nodeId1, const N2npNodeId& nodeId2)
    {
        return (!strncmp(nodeId1.name, nodeId2.name, N2npNodeNameMaxLen))
            && (nodeId1.relay == nodeId2.relay);
    }

    std::string N2npNodeId::getName(void) const
    {
        std::string s(this->name);
        return s;
    }
}
