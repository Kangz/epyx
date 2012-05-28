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
        void Dispatcher::fromN2NP(Node &node, NodeId from, const char *data, unsigned int size){
            GTTParser gttpars;
            gttpars.eat(data,size);
            GTTPacket* packet = gttpars.getPacket();
            std::string method = packet->method;
            if (method == "OPENCONNECTION"){
                Epyx::DirectConnection::Module.NodeConnect[fromID]=Epyx::DirectConnection::Module.openDirectConnection(node,from);
            }else{
                Epyx::DirectConnection::Module.NodeConnect[fromID].getMessage(method,packet->headers);
            }
            
        }
        
        
    } // namespace DirectConnection
} // namespace Epyx
