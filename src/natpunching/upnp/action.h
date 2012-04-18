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

