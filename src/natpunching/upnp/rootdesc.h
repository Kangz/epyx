/**
 * @file rootdesc.h
 * @brief UPnP rootDesc.xml management
 */

#ifndef EPYX_UPNP_ROOTDESC_H
#define EPYX_UPNP_ROOTDESC_H

#include "../../net/netquery.h"
#include "../../net/uri.h"
#include "../../parser/httpparser.h"
#include "../../../include/tinyxml/tinyxml.h"
#include <strings.h>
#include <map>

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @class Discovery
         * @brief UPnP discovery implementation with NetSelectSocket
         *
         * Answer is a service map
         */
        class RootDesc : public NetQuery<std::map<std::string, std::string> >
        {
        public:
            /**
             * @brief Constructor
             * @param URI uri to rootDesc.xml
             */
            RootDesc(const URI& uri);

            bool query();

        protected:
            std::map<std::string, std::string>* eat(const char *data, long size);

        private:
            /**
             * @brief Recursively parse RootDesc.xml to fill services
             * @param node current XML node
             * @param services filled map
             */
            void parseRootDescFile(const TiXmlElement *node,
                    std::map<std::string, std::string> *services);

            URI uri;
            HTTPParser htpars;
        };
    }
}


#endif /* EPYX_UPNP_ROOTDESC_H */

