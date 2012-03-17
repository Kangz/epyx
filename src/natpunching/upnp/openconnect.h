#ifndef OPENCONNECT_H
#define OPENCONNECT_H


#include "../../net/socket.h"

//#include <SFML/socket>
#include <string>
#include <map>

namespace Epyx {
    namespace UPNP {
        class Natpunch {
            public:
                Natpunch();
                //Socket OpenConnection(); //Opens a Listening Socket to receive a connection.
            private :
                void discover();
                void openMapPort();
                void testConnection();


                std::map<std::string,unsigned short> validIGD;
                std::string address;
                unsigned short port;
                std::string service;
                std::string endl;
        };
    }
}

#endif // OPENCONNECT_H
