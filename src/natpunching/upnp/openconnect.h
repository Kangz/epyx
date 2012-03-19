#ifndef OPENCONNECT_H
#define OPENCONNECT_H


#include "../../net/socket.h"
#include "../../net/netselect.h"
#include "igd.h"
#include "../../core/log.h"

//#include <SFML/socket>
#include <string>
#include <map>

namespace Epyx {
    namespace UPNP {
        class Natpunch {
            public:
                Natpunch();
                Address openMapPort(unsigned short localPort, unsigned short remotePort);
                //Socket OpenConnection(); //Opens a Listening Socket to receive a connection.
            private :
                void discover();
                
                void testConnection();
                void closeMapPort();
                
                IGD igd;
                Address addr;
                std::string path;
                std::string endl;
                bool success;
        };
    }
}

#endif // OPENCONNECT_H
