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
#include "netselect.h"
#include "../core/common.h"
#include "server.h"
#include "socket.h"

namespace Epyx
{

    template<class T, typename TP>
    NetSelectTCPServer<T, TP>::NetSelectTCPServer(TCPServer *srv, TP param)
    : srv(srv), param(param) {
        EPYX_ASSERT(srv != NULL);
        // Force bind
        if (!srv->isBinded())
            throw FailException("NetSelectTCPServer", "Unbinded server");
    }

    template<class T, typename TP> int NetSelectTCPServer<T, TP>::getFileDescriptor() const {
        EPYX_ASSERT(srv);
        return srv->getFd();
    }

    template<class T, typename TP> bool NetSelectTCPServer<T, TP>::read() {
        EPYX_ASSERT(srv);
        std::unique_ptr<TCPSocket> newSock = srv->accept();

        // If server is closed, return false
        if (!newSock)
            return false;

        // If fact, use a shared_ptr<Socket>
        std::shared_ptr<Socket> shSock(newSock.release());
        std::shared_ptr<T> nsSocket(new T(shSock, param));

        // Add a new socket in the NetSelect
        this->getOwner()->add(nsSocket);
        return true;
    }
}
