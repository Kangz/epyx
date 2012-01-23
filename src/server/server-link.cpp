#include "server-link.h"
#include <string.h>
#include "../core/exception.h"

namespace Epyx
{
    ServerLink::ServerLink(ServerListener *listener_, Address *srvAddr_,
                           struct sockaddr *aiClient, int sockfd_)
        :sockfd(sockfd_), listener(listener_), clientAddr(aiClient), srvAddr(srvAddr_)
    {
    }

    ServerLink::~ServerLink()
    {
        this->close();
    }

    void ServerLink::close()
    {
        if (this->sockfd >= 0) {
            ::close(this->sockfd);
            this->sockfd = -1;
        }
    }

    unsigned int ServerLink::send(const void *data, int size)
    {
        int bytes;

        if (data == NULL)
            throw FailException("ServerLink", "data is nul");
        bytes = ::send(this->sockfd, data, size, 0);
        // TODO: Implement status error (ex. Conn closed, ...)

        if (bytes == -1)
            throw ErrException("ServerLink", "send");

        return bytes;
    }

    bool ServerLink::sendAll(const void *data, int size)
    {
        int bytes;
        if (data == NULL)
            throw FailException("ServerLink", "data == NULL");

        while (size > 0) {
            bytes = this->send(data, size);
            if (!bytes)
                throw FailException("ServerLink", "Unable to send data");
            if (bytes > size)
                throw FailException("ServerLink", "More data was sent ?");
            size -= bytes;
        }
        return true;
    }

    bool ServerLink::sendText(const char *text)
    {
        if (text == NULL)
            throw FailException("ServerLink", "text == NULL");
        return this->sendAll(text, strlen(text));
    }

    unsigned int ServerLink::recv(void *data, int size)
    {
        int bytes;

        if (data == NULL)
            throw FailException("ServerLink", "data is nul");
        bytes = ::recv(this->sockfd, data, size, 0);
        // TODO: Implement status error (ex. Conn closed, ...)

        if (bytes == -1)
            throw ErrException("ServerLink", "recv");

        return bytes;
    }

    bool ServerLink::recvAll(void *data, int size)
    {
        int bytes;
        if (data == NULL)
            throw FailException("ServerLink", "data == NULL");
// TODO: feof !
        while (size > 0) {
            bytes = this->recv(data, size);
            if (!bytes)
                throw FailException("ServerLink", "Unable to recv data");
            if (bytes > size)
                throw FailException("ServerLink", "More data was recv ?");
            size -= bytes;
        }
        return true;
    }

    bool ServerLink::recvLine(std::ostream& out)
    {
        char c;
        while (this->recv(&c, 1)) {
            if (c == '\n' || c == '\r')
                return true;
            out << c;
        }
        return false;
    }
}
