#ifndef OPENCONNECT_H
#define OPENCONNECT_H


#include "../../core/socket.h"

//#include <SFML/socket>
#include <string>


namespace Epyx {
    namespace UPNP {
        class Natpunch {
            public:
                Natpunch();
                Socket OpenConnection(); //Opens a Listening Socket to receive a connection.
            private :
                void discover();
                void openMapPort();
                void testConnection();
                
                
                
                std::string address;
                unsigned short port;
                std::string service;
        };
    }
}

#endif // OPENCONNECT_H
