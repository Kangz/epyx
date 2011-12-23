/**
 * N2NP Node Identifier
 */

#ifndef EPYX_N2NP_NODE_ID_H
#define EPYX_N2NP_NODE_ID_H

#include "../core/address.h"
#include <string>

namespace Epyx
{
    const size_t N2npNodeNameMaxLen = 200;

    class N2npNodeId
    {
    private:
        char name[N2npNodeNameMaxLen + 1];
        Address relay;

    public:
        N2npNodeId();
        N2npNodeId(const Address relay_);
        N2npNodeId(const char *name_, const Address relay_);
        friend std::ostream& operator<<(std::ostream& os, const N2npNodeId nodeid);
        friend std::ostream& operator<<(std::ostream& os, const N2npNodeId *nodeid);

        friend bool operator==(const N2npNodeId& nodeId1, const N2npNodeId& nodeId2);

        friend inline bool operator!=(const N2npNodeId& nodeId1, const N2npNodeId& nodeId2)
        {
            return !(nodeId1 == nodeId2);
        }

        std::string getName(void);
    };
}

#endif /* EPYX_N2NP_NODE_ID_H */
