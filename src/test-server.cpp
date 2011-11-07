/**
 * This program tests Server class
 */

#include <iostream>
#include "exception.h"
#include "server.h"

int main()
{
    Epyx::Server *srv = NULL;
    try {
        Epyx::Server::init();
        srv = new Epyx::Server(4242, 20);
    } catch (Epyx::Exception e) {
        std::cerr << e;
    }
    if (srv)
        delete srv;
}

