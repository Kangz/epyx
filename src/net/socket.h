/**
 * Socket astraction layer
 */

#ifndef EPYX_SOCKET_H_
#define EPYX_SOCKET_H_

#include "../core/mutex.h"
#include "address.h"
#include <string>

namespace Epyx
{
    class Socket
    {
    public:
        static void init();
        static void fini();

        Socket();
        Socket(const Address& addr);
        Socket(const char* addr, unsigned short port);
        Socket(int sock, const Address &addr);
        ~Socket();

        void close();

        void setFd(int sockfd);
        int getFd();
        bool isOpened();
        void setAddress(const Address& addr);
        Address getAddress();
        void setLocalAddress(const Address& addr);
        Address getLocalAddress();

        // Senders
        virtual int send(const void *data, int size) = 0;
        void sendAll(const void *data, int size);
        void write(std::string message);

        // Recevers
        virtual int recv(void *data, int size) = 0;
        void recvAll(void *data, int size);
        bool recvLine(std::ostream& out);
        std::string read();

    protected:
        // Socket file descriptor
        int sock;
        

        // IP + port associated with the socket
        Address address;

        // Local address for the socket or "*" if unknown
        Address localAddress;

    private:
        // Global initialisation stuff
        static int is_init;
        static Mutex init_mutex;

        // Disable copy
        Socket(const Socket&);
        const Socket& operator=(const Socket&);

        // Last end-of-line caracter (\r or \n)
        char last_eol;
    };
}
#endif // EPYX_SOCKET_H_
