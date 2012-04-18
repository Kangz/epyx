/**
 * @file command.h
 * @brief UPnP Commands
 */

#ifndef EPYX_UPNP_COMMAND_H
#define EPYX_UPNP_COMMAND_H

#include <string>
#include <map>
#include "../../../include/tinyxml/tinyxml.h"
#include "../../net/netquery.h"
#include "action.h"
#include "../../parser/httpparser.h"

namespace Epyx
{
    namespace UPNP
    {
        /**
         * @brief Result of an UPnP command
         */
        typedef struct CommandResult
        {
            // HTTP status code
            int http_status;
            // Result HTTP headers
            std::map<std::string, std::string> headers;
            // Result variables
            std::map<std::string, std::string> vars;
        } CommandResult;
        /**
         * @class Command
         * @brief UPnP command interface
         */
        class Command : public NetQuery<CommandResult>
        {
        public:
            /**
             * @brief Basic constructor
             */
            Command();
            /**
             * @brief Set remote options in the constructor
             * @param addr
             * @param path
             * @param service
             */
            Command(const Address& addr, const std::string& service, const std::string& path);

            /**
             * @brief Set remote options
             * @param addr IP address
             * @param path absolute POST path for requests
             * @param service name of queried service
             */
            void setRemote(const Address& addr, const std::string& service, const std::string& path);

            /**
             * @brief Set action
             * @param action
             */
            void setAction(const ActionType action);

            /**
             * @brief Set arguments for the action
             * @param args map of arguments
             */
            void setArguments(const std::map<std::string, std::string>& args);

            /**
             * @brief Add an argument for the action
             * @param name
             * @param value
             */
            void addArgument(const std::string& name, const std::string& value);

            bool query();
        protected:
            CommandResult* eat(const char *data, long size);

        private:
            // Remote variables
            Address address;
            std::string path;
            std::string service;

            // Query
            ActionType action;
            std::map<std::string, std::string> args;

            // HTTP parser for answer
            HTTPParser htpars;
        };
    }
}
#endif /* EPYX_UPNP_COMMAND_H */
