/**
 * This program tests Server class
 */

#include <iostream>
#include <sstream>
#include <strings.h>
#include "core/exception.h"
#include "server/server.h"
#include "server/server-link.h"

bool TestServer(Epyx::ServerLink& link)
{
    const char *line;

    std::ostringstream hello;
    hello << "Hello, you address is " << link.clientAddr << " !\n";
    link.sendText(hello.str().c_str());
    link.sendText("Type QUIT to quit the server\n");
    link.sendText("Type EXIT to exit the session\n");
    link.sendText("Type PAN to kill the session\n");
    while (true) {
        std::ostringstream out;
        if (!link.recvLine(out)) {
            std::cout << "end\n";
            return true;
        }
        std::cout << "[RECV@" << link.clientAddr << "] " << out.str() << "\n";
        line = out.str().c_str();
        if (!strcasecmp(line, "quit"))
            return false;
        else if (!strcasecmp(line, "exit"))
            return true;
        else if (!strcasecmp(line, "pan"))
            throw Epyx::FailException("test-server", "A client tried to kill me");
        else if (!strcasecmp(line, "o<"))
            link.sendText("PAN !\n");
    }
    return true;
}

int main()
{
    Epyx::Server *srv = NULL;
    try {
        Epyx::Server::init();
        srv = new Epyx::Server(4242, 20, TestServer);
    } catch (Epyx::Exception e) {
        std::cerr << e;
    }
    if (srv)
        delete srv;
}

