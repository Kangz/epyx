#include "module.h"
#include "dispatcher.h"

namespace Epyx
{
    namespace DirectConnection
    {

        std::shared_ptr<OpenConnection> Module::openDirectConnection(const std::shared_ptr<N2NP::Node>& node,
            const N2NP::NodeId& remoteHost) { //As Client
            std::shared_ptr<OpenConnection> opener(new OpenConnection(node, remoteHost));
            return opener;
        }

        std::shared_ptr<OpenConnection> Module::openDirectConnectionServer(const std::shared_ptr<N2NP::Node>& node,
            const N2NP::NodeId&remoteHost) { //As Server
            std::shared_ptr<OpenConnection> opener(new OpenConnection(node, remoteHost, false));
            return opener;
        }
    }
}
