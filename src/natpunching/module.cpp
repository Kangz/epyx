#include "module.h"
#include "dispatcher.h"
namespace Epyx {
    namespace DirectConnection {
        atom::Map<N2NP::NodeId,OpenConnection*> Module::NodeConnect;
        
        void Module::init(N2NP::Node &node)
        {
            std::shared_ptr<Dispatcher> dispatch(new Dispatcher);
            node.addModule("DIRECTCONNECTION", dispatch);
        }
        OpenConnection* Module::openDirectConnection(N2NP::Node &node, N2NP::NodeId &remoteHost){ //As Client
            OpenConnection* opener = new OpenConnection(node,remoteHost);
            opener->start();
            return opener;
        }
        OpenConnection* Module::openDirectConnectionServer(N2NP::Node &node, N2NP::NodeId &remoteHost){ //As Server
            OpenConnection* opener = new OpenConnection(node,remoteHost,false);
            opener->start();
            return opener;
        }
        
    } // namespace DirectConnection
} // namespace Epyx
