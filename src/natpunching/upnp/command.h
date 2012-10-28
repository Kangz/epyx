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
            Command(const SockAddress& addr, const std::string& service, const std::string& path);

            /**
             * @brief Set remote options
             * @param addr IP address
             * @param path absolute POST path for requests
             * @param service name of queried service
             */
            void setRemote(const SockAddress& addr, const std::string& service, const std::string& path);

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
            std::unique_ptr<CommandResult> eat(const byte_str& data);

        private:
            // Remote variables
            SockAddress address;
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
