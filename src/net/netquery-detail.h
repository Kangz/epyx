#include "../core/common.h"
#include <sys/select.h>
#include <sys/time.h>

namespace Epyx
{

    template<typename T> NetQuery<T>::NetQuery()
    : sock(NULL), dynamicSock(false) {
    }

    template<typename T> NetQuery<T>::NetQuery(Socket& sock)
    : sock(&sock), dynamicSock(false) {
    }

    template<typename T> NetQuery<T>::NetQuery(Socket *psock)
    : sock(psock), dynamicSock(true) {
    }

    template<typename T> NetQuery<T>::~NetQuery() {
        this->close();
    }

    template<typename T> void NetQuery<T>::setSocket(Socket& sock) {
        EPYX_ASSERT(this->sock == NULL);
        this->sock = &sock;
    }

    template<typename T> void NetQuery<T>::close() {
        if (sock != NULL) {
            sock->close();
            if (dynamicSock)
                delete sock;
            sock = NULL;
        }
    }

    template<typename T> T* NetQuery<T>::answer(const Timeout& timeout) {
        struct timeval tv;
        fd_set rfds;
        const int size = 4096;
        char data[size];
        EPYX_ASSERT(sock != NULL);
        int sockfd = sock->getFd();
        if (sockfd < 0)
            return NULL;

        // select() loop
        while (!timeout.hasExpired()) {
            // Wait with select()
            FD_ZERO(&rfds);
            FD_SET(sockfd, &rfds);
            tv = timeout.remainingTimeval();
            int status = select(sockfd + 1, &rfds, NULL, NULL, &tv);
            if (status < -1)
                throw ErrException("NetSelect", "select");

            // Read data
            if (FD_ISSET(sockfd, &rfds)) {
                int recvSize = sock->recv(data, size);

                try {
                    // Eat data and return any answer
                    // Note: if recvSize = 0, this is the last call to eat()
                    T* ans = this->eat(data, recvSize);
                    if (ans != NULL)
                        return ans;

                    // Close on End-Of-File
                    if (recvSize == 0) {
                        this->close();
                        return NULL;
                    }
                } catch (Exception e) {
                    log::error << e << log::endl;
                    log::error << "Closing socket to " << sock->getAddress() <<
                            " due to an exception" << log::endl;
                    this->close();
                    return NULL;
                }
            }
        }
        return NULL;
    }

    template<typename T> bool NetQuery<T>::answerIn(const Timeout& timeout, T *ans) {
        T *result = this->answer(timeout);
        if (result == NULL)
            return false;
        if (ans != NULL)
            *ans = *result;
        delete result;
        return true;
    }

    template<typename T> T* NetQuery<T>::queryAnswer(const Timeout& timeout) {
        if (!this->query()) {
            log::error << "NetQuery: Unable to send query" << log::endl;
            return NULL;
        }
        return this->answer(timeout);
    }

    template<typename T> bool NetQuery<T>::queryAnswerIn(const Timeout& timeout, T *ans) {
        if (!this->query()) {
            log::error << "NetQuery: Unable to send query" << log::endl;
            return false;
        }
        return this->answerIn(timeout, ans);
    }

    template<typename T> Socket& NetQuery<T>::socket() const {
        EPYX_ASSERT(sock != NULL);
        return *sock;
    }
}
