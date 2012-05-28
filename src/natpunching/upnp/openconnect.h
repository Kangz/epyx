/**
 * @file openconnect.h
 * @brief Open a connection through NAT punching
 */
#ifndef OPENCONNECT_H
#define OPENCONNECT_H

#include "../../net/socket.h"
#include "../../net/netselect.h"
#include "../../net/uri.h"
#include "../../core/log.h"
#include "igd.h"

#include <string>
#include <map>

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Natpunch
         * @brief UPnP Nat Punghing implementation
         */
        class Natpunch
        {
        public:
            /**
             * @brief constructor
             */
            Natpunch();
            /**
             * @brief destructor
             */
            ~Natpunch();

            /**
             * @brief creating a port mapping behind a NAT
             * @param localPort
             * @param remotePort
             * @return Address of the NAT router
             */
            const Address openMapPort(unsigned short localPort, unsigned short remotePort =0);
            //Socket OpenConnection(); //Opens a Listening Socket to receive a connection.
        private:
            void testConnection();
            void closeMapPort();

            IGD *igd;
            URI uri;
            bool success;
        };
    }
}

#endif // OPENCONNECT_H
