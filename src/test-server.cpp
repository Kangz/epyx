/**
 * This program tests Server class
 */

#include <iostream>
#include <sstream>
#include <strings.h>
#include "core/common.h"
#include "net/tcpserverthread.h"

class TestServer : public Epyx::TCPServerThread
{
public:
    TestServer(int port, std::string threadname)
    :TCPServerThread(port, 20, threadname)
    {
    }

    void runSocket(Epyx::Socket& sock)
    {
        const char *line;

        Epyx::log::debug << "[" << sock.getAddress() << "] "
            << "Incoming for " << this->getAddress() << Epyx::log::endl;

        std::ostringstream hello;
        hello << "Hello, you address is " << sock.getAddress() << " !\n";
        sock.write(hello.str());
        sock.write("Type QUIT to quit the server\n");
        sock.write("Type EXIT to exit the session\n");
        sock.write("Type PAN to kill the session\n");
        while (true) {
            std::ostringstream out;
            if (!sock.recvLine(out)) {
                Epyx::log::debug << "[" << sock.getAddress() << "] "
                    << "Remote disconnected" << Epyx::log::endl;
                return;
            }
            Epyx::log::debug << "[" << sock.getAddress() << "-RECV] "
                << out.str() << Epyx::log::endl;
            line = out.str().c_str();
            if (!strcasecmp(line, "quit")) {
                this->close();
                //this->term ??? to end server
                return;
            } else if (!strcasecmp(line, "exit"))
                return;
            else if (!strcasecmp(line, "pan"))
                throw Epyx::FailException("test-server", "A client tried to kill me");
            else if (!strcasecmp(line, "o<"))
                sock.write("PAN !\n");
            else {
                // Mirror
                out << '\n';
                sock.write("Mirror: ");
                sock.write(out.str());
            }
        }
        return;
    }
};

void test_server()
{
    Epyx::log::debug << "Starting server at port 4242..." << Epyx::log::endl;
    TestServer t(4242, "Server");
    Epyx::log::debug << "(server) RUN!" << Epyx::log::endl;
    t.start();
    t.wait();
    Epyx::log::debug << "Server thread has terminated" << Epyx::log::endl;
}

int main()
{
    Epyx::Thread::init();
    Epyx::log::init(Epyx::log::CONSOLE, "");
    Epyx::Socket::init();
    try {
        test_server();
    } catch (Epyx::Exception e) {
        Epyx::log::error << e << Epyx::log::endl;
    }
    Epyx::log::flushAndQuit();
}

