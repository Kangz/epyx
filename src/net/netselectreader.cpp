#include "netselectreader.h"
#include "../core/common.h"

namespace Epyx
{
    NetSelectReader::NetSelectReader()
        :owner(NULL)
    {
    }

    NetSelectReader::~NetSelectReader()
    {
    }

    NetSelect* NetSelectReader::getOwner()
    {
        EPYX_ASSERT(owner != NULL);
        return owner;
    }

    void NetSelectReader::setOwner(NetSelect *ns)
    {
        EPYX_ASSERT(owner == NULL);
        owner = ns;
    }
}
