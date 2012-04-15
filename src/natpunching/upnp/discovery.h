#ifndef EPYX_UPNP_DISCOVERY_H
#define EPYX_UPNP_DISCOVERY_H

#include "../../net/netselectsocket.h"
#include "../../parser/httpparser.h"
#include "../../net/udpsocket.h"

namespace Epyx {
    namespace UPNP {
        
        class Discovery : public NetSelectSocket
        {
            public:
                Discovery(UDPSocket& s);
                void eat(const char *data, long size);
                Address getAddress();
                std::string getPath();
            private:
                Address addr;
                std::string rootPath;
                GTTPacket *pkt;
                HTTPParser htpars;
        };
        
    } // namespace UPNP
} // namespace Epyx

#endif // EPYX_UPNP_DISCOVERY_H
