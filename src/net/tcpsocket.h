#ifndef EPYX_TCPSOCKET_H
#define EPYX_TCPSOCKET_H

#include "socket.h"

namespace Epyx
{
    /**
     * @class TCPSocket
     *
     * @brief TCP socket abstraction layer
     */
    class TCPSocket : public Socket
    {
    public:
        TCPSocket();
        TCPSocket(const Address&);
        TCPSocket(int sock, const Address &addr);

        /**
         * @brief Send data through the socket
         *
         * @param data sent data
         * @param size size of data to be sent
         * @return number of sent bytes
         */
        int send(const void *data, int size);

        /**
         * @brief Receive data from the socket
         *
         * @param data received data buffer
         * @param size size of data buffer
         * @return number of received bytes
         */
        int recv(void *data, int size);
    private:
        bool connect();
        bool isConnected;
    };

}
#endif // EPYX_UDPSOCKET_H
