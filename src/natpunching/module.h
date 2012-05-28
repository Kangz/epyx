#ifndef EPYX_DIRECTCONNECTION_MODULE_H
#define EPYX_DIRECTCONNECTION_MODULE_H
#include "../core/atom/map.h"
#include "openconnection.h"
#include "../net/address.h"
#include "../n2np/nodeid.h"
#include "../net/netselect.h"
#include <stack>
namespace Epyx {
    namespace DirectConnection {
        
        class Module
        {
            public:
                // Will Call DirectConnection::OpenConnection class which opens an independant Thread
                // Use to be the Client
                static OpenConnection openDirectConnection(N2NP::Node &node, N2NP::NodeId &remoteHost); 
                // Use to be the server
                static OpenConnection openDirectConnectionServer(N2NP::Node &node, N2NP::NodeId &remoteHost);
                static Epyx::atom::Map<Epyx::N2NP::NodeId,OpenConnection> NodeConnect;
                static void init(Node &node);
                //static Epyx::NetSelect ConnectionListeners;
            private:
                
        };
        
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_MODULE_H
