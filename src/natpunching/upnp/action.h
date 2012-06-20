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
 * @file action.h
 * @brief Manage UPnP actions
 */

#ifndef EPYX_UPNP_ACTION_H
#define EPYX_UPNP_ACTION_H

namespace Epyx
{
    namespace UPNP
    {
        typedef enum ActionType
        {
            UPNP_ACTION_CONNTYPE = 1,
            UPNP_ACTION_GET_EXT_IP,
            UPNP_ACTION_ADDPORTMAP,
            UPNP_ACTION_DELPORTMAP,
            UPNP_ACTION_NUMENTRIES_PORTMAP,
            UPNP_ACTION_SPECIFICPORTMAP,
            UPNP_ACTION_GET_GEN_PORTMAP,
            UPNP_ACTION_LIST_PORTMAP,
            UPNP_ACTION_GET_STATINFO
        } ActionType;

        class Action {
        public:
            /**
             * @brief Get a string for an action type
             * @param type
             * @return const char*, name of the action
             */
            static const char* toString(ActionType type);

            /**
             * @brief Tell if arguments are mandatory for this action type
             * @param type
             * @return true if arguments are expected
             */
            static bool needArgs(ActionType type);
        };
    }
}

#endif /* EPYX_UPNP_ACTION_H */

