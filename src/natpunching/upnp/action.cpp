#include "action.h"
#include "../../core/exception.h"

namespace Epyx
{
    namespace UPNP
    {
        const char* Action::toString(ActionType type) {
            switch (type) {
                case UPNP_ACTION_CONNTYPE:
                    return "GetConnectionTypeInfo";
                case UPNP_ACTION_GET_EXT_IP:
                    return "GetExternalIPAddress";
                case UPNP_ACTION_ADDPORTMAP:
                    return "AddPortMapping";
                case UPNP_ACTION_DELPORTMAP:
                    return "DeletePortMapping";
                case UPNP_ACTION_NUMENTRIES_PORTMAP:
                    return "GetPortMappingNumberOfEntries";
                case UPNP_ACTION_SPECIFICPORTMAP:
                    return "GetSpecificPortMappingEntry";
                case UPNP_ACTION_LIST_PORTMAP:
                    return "GetListOfPortMappings";
                case UPNP_ACTION_GET_GEN_PORTMAP:
                    return "GetGenericPortMappingEntry";
                case UPNP_ACTION_GET_STATINFO:
                    return "GetStatusInfo";
                default:
                    throw FailException("UPnP::Action::str", "Unknown type");
            }
        }

        bool Action::needArgs(ActionType type) {
            switch (type) {
                case UPNP_ACTION_CONNTYPE:
                case UPNP_ACTION_GET_EXT_IP:
                case UPNP_ACTION_NUMENTRIES_PORTMAP:
                case UPNP_ACTION_GET_STATINFO:
                    return false;
                case UPNP_ACTION_ADDPORTMAP:
                case UPNP_ACTION_DELPORTMAP:
                case UPNP_ACTION_SPECIFICPORTMAP:
                case UPNP_ACTION_LIST_PORTMAP:
                case UPNP_ACTION_GET_GEN_PORTMAP:
                    return true;
                default:
                    throw FailException("UPnP::Action::arg", "Unknown type");
            }
        }
    }
}
