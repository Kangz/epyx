/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
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

