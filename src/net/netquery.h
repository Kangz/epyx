/**
 * @file netquery.h
 * @brief Use sockets for a single query
 */

#ifndef EPYX_NETQUERY_H
#define EPYX_NETQUERY_H

#include "socket.h"
#include "../core/timeout.h"

namespace Epyx
{
    /**
     * @class NetQuery
     * @brief Manage a socket which is used only once
     *
     * Usage:
     *  1- Instantiate a NetQuery object
     *  2- Set socket (UDP or TCP, which has an address)
     *  3- Send query
     *  4- Wait for receiving answer (with a timeout)
     *  5- Close everything
     *
     * Methods:
     *  1- Constructor
     *  2- setSocket(sock) or constructor
     *  3- abstract query() method which calls socket().send()
     *  4- T* answer = answer(timeout) which calls abstract eat(data) on
     *     each received data.
     *  5- Destructor or close()
     */
    template<typename T>class NetQuery
    {
    public:
        /**
         * @brief Empty constructor
         */
        NetQuery();

        /**
         * @brief Constructor with a reference
         *
         * Create a new NetQuery with a socket whose allocation is NOT
         * managed by this object.
         *
         * @param sock socket reference
         */
        NetQuery(Socket &sock);

        /**
         * @brief Constructor used with new Socket()
         *
         * Create a new NetQuery with a socket whose allocation is
         * managed by this object: the socket is DELETED in the destructor.
         *
         * @param psock socket pointer
         */
        NetQuery(Socket *psock);

        /**
         * @brief Desctructor
         */
        virtual ~NetQuery();

        /**
         * @brief Set socket
         * @param sock
         */
        void setSocket(Socket& sock);

        /**
         * @brief Close everything
         */
        void close();

        /**
         * @brief send query
         * @return false if there is a problem
         */
        virtual bool query() = 0;

        /**
         * @brief Wait for an answer
         * @param timeout maximum number of seconds to wait for an answer
         * @return the answer, or NULL if there is a timeout or an error
         */
        T* answer(const Timeout& timeout);

        /**
         * @brief Same as answer(), but modify the given reference
         * @param timeout
         * @param ans pointer wher answer is copied
         * @return true on success
         */
        bool answerIn(const Timeout& timeout, T *ans);

        /**
         * @brief query() and answer() in one call
         * @param timeout
         * @return answer
         */
        T* queryAnswer(const Timeout& timeout);
        /**
         * @brief query() and answerIn() in one call
         * @param timeout
         * @param ans
         * @return answer
         */
        bool queryAnswerIn(const Timeout& timeout, T *ans);

    protected:
        /**
         * @brief Get internal socket
         * @return sock
         */
        Socket& socket() const;

        /**
         * @brief Eat data and return an answer once it is ready
         * @param data data to eat
         * @param size available size
         * @return NULL if data is incomplete, the newly-allocated answer otherwise
         */
        virtual T* eat(const char *data, long size) = 0;

    private:
        // Disable copy
        NetQuery(const NetQuery&);
        const NetQuery& operator=(const NetQuery&);

        Socket *sock;
        bool dynamicSock;
    };
}

#include "netquery-detail.h"

#endif /* EPYX_NETQUERY_H */
