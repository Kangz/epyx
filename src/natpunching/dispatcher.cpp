#include "dispatcher.h"
#include "../parser/gttparser.h"
#include "../parser/gttpacket.h"
#include "../core/string.h"
#include "openconnection.h"
#include "../n2np/nodeid.h"
#include <string>

namespace Epyx
{
    namespace DirectConnection
    {

        void Dispatcher::addModule(const std::shared_ptr<N2NP::Node>& node) {
            EPYX_ASSERT(node);
            std::shared_ptr<Dispatcher> dispatch(new Dispatcher(node));
            node->addModule("DIRECTCONNECTION", dispatch);
        }

        void Dispatcher::fromN2NP(N2NP::Node& node, N2NP::NodeId from, const byte_str& data) {
            GTTParser gttpars;
            gttpars.eat(data);
            std::unique_ptr<GTTPacket> packet = gttpars.getPacket();
            if (packet->method == "OPENCONNECTION") {
                // Someone is asking to open a connection to him
                // Create a OpenConnection client instance
                std::shared_ptr<OpenConnection> oconn(new OpenConnection(this->node, from, true));
                std::lock_guard<std::mutex> lock(nodeConnectMutex);
                nodeConnect[from] = oconn;
            } else {
                std::shared_ptr<OpenConnection> oconn;
                {
                    std::lock_guard<std::mutex> lock(nodeConnectMutex);
                    // Find an iterator
                    auto it = nodeConnect.find(from);
                    if (it == nodeConnect.end()) {
                        throw MinorException("DirectConnection::Dispatcher::fromN2NP",
                            "No direct connection from specified ID");
                    }
                    oconn = it->second;
                }
                oconn->getMessage(packet->method, packet->headers);
            }
        }

        Dispatcher::Dispatcher(const std::shared_ptr<N2NP::Node>& node)
        :node(node) {
        }
    } // namespace DirectConnection
} // namespace Epyx
