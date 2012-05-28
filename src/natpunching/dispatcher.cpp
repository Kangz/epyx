#include "dispatcher.h"
#include "../parser/gttparser.h"
#include "../parser/gttpacket.h"
#include "../core/string.h"
#include "module.h"
#include "openconnection.h"
#include "../n2np/nodeid.h"
#include <string>

namespace Epyx {
    namespace DirectConnection {
        void Dispatcher::fromN2NP(N2NP::Node &node, N2NP::NodeId from, const char *data, unsigned int size){
            GTTParser gttpars;
            gttpars.eat(data,size);
            GTTPacket* packet = gttpars.getPacket();
            std::string method = packet->method;
            if (method == "OPENCONNECTION"){
                DirectConnection::Module::NodeConnect.set(from, DirectConnection::Module::openDirectConnection(node,from));
            }else{
                OpenConnection *oconn = DirectConnection::Module::NodeConnect.get(from, NULL);
                if (oconn == NULL)
                    throw MinorException("DirectConnection::Dispatcher::fromN2NP",
                        "No direct connection from specified ID");
                oconn->getMessage(method,packet->headers);
            }
            
        }
        
        
    } // namespace DirectConnection
} // namespace Epyx
