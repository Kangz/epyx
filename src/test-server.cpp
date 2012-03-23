/**
 * This program tests Server class
 */

#include <iostream>
#include <sstream>
#include <strings.h>
#include "core/common.h"
#include "net/tcpserverthread.h"
#include "net/netselect.h"
#include "net/netselectsocket.h"
#include "net/netselecttcpserver.h"

/**
 * @class TestNetSelectSocket
 *
 * @brief Test Epyx::NetSelectSocket class
 */
class TestNetSelectSocket : public Epyx::NetSelectSocket
{
public:
    /**
     * @brief Welcome an incomming connection
     * @param sock Incomming
     */
    TestNetSelectSocket(Epyx::Socket *sock)
        :NetSelectSocket(sock)
    {
        Epyx::log::debug << "[" << socket().getAddress() << "] " <<
            "Incoming for " << socket().getLocalAddress() << Epyx::log::endl;

        // Motd
        std::ostringstream hello;
        hello << "Hello, you address is " << socket().getAddress() << " !\n" <<
            "Type EXIT or QUIT to exit the session\n" <<
            "Type PAN to kill the session\n";
        socket().write(hello.str());
    }
    /**
     * @brief End a connection
     */
    ~TestNetSelectSocket()
    {
         Epyx::log::debug << "[" << socket().getAddress() << "] End" <<
             Epyx::log::endl;
    }

    /**
     * @brief Treat received data
     */
    void eat(const char *data, long size)
    {
        std::string strline(data, size);
        Epyx::String::trim(strline);
        Epyx::log::debug << "[" << socket().getAddress() << "-RECV] " <<
           strline << Epyx::log::endl;

        const char *line = strline.c_str();

        if (!strcasecmp(line, "quit") || !strcasecmp(line, "exit")) {
            // Close socket
            this->socket().close();
        } else if (!strcasecmp(line, "pan")) {
            throw Epyx::FailException("test-server", "A client tried to kill me");
        } else if (!strcasecmp(line, "o<")) {
            socket().write("PAN !\n");
        } else {
            // Mirror
            socket().write("Mirror: ");
            socket().sendAll(data, size);
        }
    }
};

int main()
{
    Epyx::Thread::init();
    Epyx::log::init(Epyx::log::CONSOLE, "");
    Epyx::Socket::init();
    try {
        // Start select() thread
        Epyx::NetSelect selectThread(20, "SelectWorker");
        // Do NOT delete tcpServer : it is deleted by NetSelectTCPServer
        Epyx::TCPServer *tcpServer = new Epyx::TCPServer(4242, selectThread.getNumWorkers());
        Epyx::log::debug << "ServerWorker listening at " << tcpServer->getAddress() <<
            Epyx::log::endl;
        selectThread.add(new Epyx::NetSelectTCPServer<TestNetSelectSocket>(tcpServer));
        selectThread.setName("NetSelect");
        Epyx::log::debug << "Start " << selectThread.getThisName() <<
            Epyx::log::endl;
        selectThread.start();

        // Wait thread
        selectThread.wait();
        Epyx::log::debug << "Server thread has terminated" << Epyx::log::endl;
    } catch (Epyx::Exception e) {
        Epyx::log::error << e << Epyx::log::endl;
    }
    Epyx::log::flushAndQuit();
}

