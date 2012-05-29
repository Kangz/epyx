#ifndef EPYX_DIRECTCONNECTION_LISTENER_H
#define EPYX_DIRECTCONNECTION_LISTENER_H
#include "../core/condition.h"
#include "../net/netselectsocket.h"
#include "../net/uri.h"
#include "../net/tcpsocket.h"
#include "../net/tcpserver.h"
#include "../n2np/node.h"

namespace Epyx {
    namespace DirectConnection {
        
        class Listener : public Thread
        {
            public:
                Listener(TCPServer * srv);
                TCPSocket * getSocket();
                Address getAddress();
                Address getLocalAddress();
                char* getData();
                bool hasAccepted();
            protected:
                void run();
            private:
                bool hasAccept;
                char* data;
                TCPServer* srv;
                TCPSocket * sock;
        };
        
    } // namespace DirectConnection
} // namespace Epyx

#endif // EPYX_DIRECTCONNECTION_LISTENER_H
