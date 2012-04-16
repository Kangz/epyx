/**
 * @file discovery.h
 * @brief UPnP discovery implementation
 */
#ifndef EPYX_UPNP_DISCOVERY_H
#define EPYX_UPNP_DISCOVERY_H

#include "../../core/condition.h"
#include "../../net/netselectsocket.h"
#include "../../parser/httpparser.h"

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Discovery
         * @brief UPnP discovery implementation with NetSelectSocket
         */
        class Discovery : public NetSelectSocket
        {
        public:
            /**
             * @brief Constructor, send a search request through a UDP socket
             */
            Discovery();

            /**
             * @brief Wait for an answer
             * @param t timeout in seconds
             * @return true if there is an answer, false if time out happened
             */
            bool waitAnswer(int timeout);

            /**
             * @brief Get address (IP:port) of the answer
             * @return a copy of this->addr
             */
            Address getAddress() const;

            /**
             * @brief Get absolute path of the answer
             * @return a copy of this->path
             */
            std::string getPath() const;

        protected:
            void eat(const char *data, long size);

        private:
            Address addr;
            std::string rootPath;
            bool hasAnswer;
            Condition answerCond;
            HTTPParser htpars;
        };

    } // namespace UPNP
} // namespace Epyx

#endif // EPYX_UPNP_DISCOVERY_H
