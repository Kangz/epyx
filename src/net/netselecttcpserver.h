/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file netselecttcpserver.h
 * @brief Implementation of NetSelectReader on TCP server
 */

#ifndef EPYX_NETSELECTTCPSERVER_H
#define EPYX_NETSELECTTCPSERVER_H

#include "netselectsocket.h"
#include "tcpserver.h"
#include "tcpsocket.h"

namespace Epyx
{
    /**
     * @class NetSelectTCPServer
     *
     * @brief Manage a TCPServer in NetSelect
     *
     * @param T is a class which derives from NetSelectSocket and which is
     * intended to be instanced for each incomming connection.
     *
     * @param TP (for param) is the type the second argument of the constructor
     * of TSelSock
     *
     * TSelSock needs to have a constructor TSelSock(Socket *sock, TParam *param)
     */
    template<class T, typename TP>class NetSelectTCPServer : public NetSelectReader
    {
    public:
        /**
         * @brief Create a NetSelectTCPServer with a new TCPServer
         *
         * NOTE: TCPServer *srv is DELETED by the desctructor
         */
        NetSelectTCPServer(TCPServer *srv, TP param);
        ~NetSelectTCPServer();

        int getFileDescriptor() const;

        bool read();

    private:
        /**
         * @brief Monitored TCPServer
         */
        TCPServer *srv;

        // Constructor parameter
        TP param;
    };
}

#include "netselecttcpserver-detail.h"

#endif /* EPYX_NETSELECTTCPSERVER_H */
