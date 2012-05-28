#ifndef EPYX_DIRECTCONNECTION_DISPATCHER_H
#define EPYX_DIRECTCONNECTION_DISPATCHER_H

#include "../n2np/module.h"

namespace Epyx {
    namespace DirectConnection {
        
        class Dispatcher : public N2NP::Module
        {
            public:
                void fromN2NP(N2NP::Node &node, N2NP::NodeId from, const char *data, unsigned int size);
        };
        
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_DISPATCHER_H
