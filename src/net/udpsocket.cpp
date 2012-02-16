#include "udpsocket.h"
#include "../core/exception.h"
#include "../core/assert.h"

#define BUF_SIZE 4096
namespace Epyx{

    UDPSocket::UDPSocket(bool reply)
        :Socket("239.255.255.250", 0), reply(reply)
    {
        this->sock = ::socket(PF_INET, SOCK_DGRAM, 0);
        if (this->sock == -1)
            throw ErrException("UDPSocket", "socket");

        if (reply) {
            this->ans_sock = ::socket(PF_INET, SOCK_DGRAM, 0);
            if (this->sock == -1)
                throw ErrException("UDPSocket", "socket[ans]");
        }
    }

    void UDPSocket::connect(){
        Socket::connect();
        if (reply){
            struct addrinfo hints;
            struct addrinfo *result, *rp;
            struct sockaddr_storage peer_addr;
            socklen_t peer_addr_len;
            ssize_t nread;
            char buf[BUF_SIZE];

            memset(&hints, 0, sizeof(struct addrinfo));
            hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
            hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
            hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
            hints.ai_protocol = 0;          /* Any protocol */
            hints.ai_canonname = NULL;
            hints.ai_addr = NULL;
            hints.ai_next = NULL;
            char charport[10];
            sprintf(charport,"%d",this->address.getPort());
            int s=getaddrinfo(NULL, charport , &hints, &result);
            if (s != 0) {
                throw ErrException("UDPSocket","getaddrinfo" );
            }

            /*  getaddrinfo() returns a list of address structures.
                Try each address until we successfully bind(2).
                If socket(2) (or bind(2)) fails, we (close the socket
                and) try the next address. */

            for (rp = result; rp != NULL; rp = rp->ai_next) {
                 ans_sock = ::socket(rp->ai_family, rp->ai_socktype,
                         rp->ai_protocol);
                 if (ans_sock == -1)
                     continue;

                if (::bind(ans_sock, rp->ai_addr, rp->ai_addrlen) == 0)
                     break;                  /* Success */

                ::close(ans_sock);
             }

            if (rp == NULL) {               /* No address succeeded */
                throw ErrException("UDPSocket", "bind : No one to bind with");
            }

            freeaddrinfo(result);           /* No longer needed */
        }
    }

    void UDPSocket::close(){
        Socket::close();
        if (reply){
            ::shutdown(ans_sock,SHUT_RDWR);
            ::close(ans_sock);
        }
    }
    int UDPSocket::recv(void *data, int size) {
        int bytes;
        EPYX_ASSERT(data != NULL);
        bytes = ::recv(this->ans_sock, data, size, 0);
        // TODO: Implement status error (ex. Conn closed, ...)
        if (bytes == -1)
            throw ErrException("UDPSocket", "recv");
        return bytes;
    }
}
