/**
 * Basic Internet server class
 */
#ifndef EPYX_SERVER_H
#define EPYX_SERVER_H

#include "address.h"

namespace Epyx
{
    class Server
    {
    public:
        Server(unsigned short port);
        ~Server();

        void setPort(unsigned short port_);
        const Address& getAddress();

        /**
         * @brief Start the server (socket + bind + listen)
         */
        virtual bool bind() = 0;

        /**
         * @brief Tell wether server is binded or not
         */
        inline bool isBinded()
        {
            return (sockfd >= 0);
        }

        /**
         * @brief Stop the server (shutdown + close)
         */
        void close();

        /**
         * @brief Return internal file descriptor
         */
        inline int getFd()
        {
            return sockfd;
        }

    protected:
        Address address;
        unsigned short port;
        int sockfd;
        // Another thread tells the server to shut down
        bool running;

        /**
         * Common code for TCP and UDP bind
         * @param socktype: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
         * @return true on success
         */
        bool _internal_bind(int socktype);

    private:
         // Disable copy
        Server(const Server&);
        const Server& operator=(const Server&);
   };

}
#endif // EPYX_SERVER_H
