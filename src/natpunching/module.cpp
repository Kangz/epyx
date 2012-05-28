#include "module.h"
#include "dispatcher.h"
namespace Epyx {
    namespace DirectConnection {
        
        void Module::init(N2NP::Node &node)
        {
            Dispatcher * dispatch = new dispatcher;
            node.addModule("DIRECTCONNECTION", dispatch);
        }
        OpenConnection Module::openDirectConnection(N2NP::Node &node, N2NP::NodeId &remoteHost){ //As Client
            OpenConnection opener(node,remoteHost);
            opener.start();
            return opener;
        }
        OpenConnection Module::openDirectConnectionServer(N2NP::Node &node, N2NP::NodeId &remoteHost){ //As Server
            OpenConnection opener(node,remoteHost,false);
            opener.start();
            return opener;
        }
        
    } // namespace DirectConnection
} // namespace Epyx
