#ifndef EPYX_DHT_PEER_H
#define EPYX_DHT_PEER_H

#include "../n2np/n2np-nodeid.h"
#include "id.h"
namespace Epyx
{
namespace DHT
{

    class Peer {
        public:
            Id id;
            N2npNodeId n2npId;
            time_t lastReceiveTime;
            
            void serialize(std::ostream& o);
            void unserialize(std::istream& i);
    };

}
}
#endif /*EPYX_DHT_PEER_H*/
