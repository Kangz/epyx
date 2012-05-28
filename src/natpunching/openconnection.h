#ifndef EPYX_DIRECTCONNECTION_OPENCONNECTION_H
#define EPYX_DIRECTCONNECTION_OPENCONNECTION_H
#include "../core/thread.h"
#include "../net/tcpsocket.h"
#include <string>
#include <map>
#include "../n2np/nodeid.h"
#include "../n2np/node.h"
namespace Epyx {
    namespace DirectConnection {
        typedef enum state{STATE_CLIENT, 
                           STATE_SERVER
                          }state;
        typedef enum method{DIRECT,
                            UPNP
                           }method;
        class OpenConnection : public Thread
        {
            public:
                //Initialising the state Machine
                OpenConnection(N2NP::Node& node, N2NP::NodeId & remoteHost, bool client = true);
                //Avancing from a state to another
                void getMessage(std::string command, std::map<std::string,std::string> headers);
            protected:
                void run();
            private:
                void serverStateOpen();
                N2NP::NodeId remoteHost;
                state etat;
                bool client_tried;
                bool server_tried;
                method tested_method;
                N2NP::Node node;
                TCPSocket socket;
        };
        
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_OPENCONNECTION_H
