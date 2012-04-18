/**
 * @file discovery.h
 * @brief UPnP discovery implementation
 */
#ifndef EPYX_UPNP_DISCOVERY_H
#define EPYX_UPNP_DISCOVERY_H

#include "../../core/condition.h"
#include "../../net/netquery.h"
#include "../../net/uri.h"
#include "../../parser/httpparser.h"

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Discovery
         * @brief UPnP discovery implementation with NetSelectSocket
         */
        class Discovery : public NetQuery<URI>
        {
        public:
            /**
             * @brief Constructor
             */
            Discovery();

            /**
             * @brief Send a search request through a UDP socket
             */
            bool query();

        protected:
            URI* eat(const char *data, long size);

        private:
            HTTPParser htpars;
        };
    }
}

#endif /* EPYX_UPNP_DISCOVERY_H */
