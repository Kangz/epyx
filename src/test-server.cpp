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
 * @class TestServerThread
 *
 * @brief Test Epyx::TCPServerThread class
 */
class TestServerThread : public Epyx::TCPServerThread
{
public:
    TestServerThread(unsigned short port, unsigned int nbConn = 20)
        :TCPServerThread(port, nbConn)
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

/**
 * @class TestNetSelectSocket
 *
 * @brief Test Epyx::NetSelectSocket class
 */
class TestNetSelectSocket : public Epyx::NetSelectSocket
{
public:
    TestNetSelectSocket(Epyx::Socket *sock)
        :NetSelectSocket(sock)
    {
        Epyx::log::debug << "[" << socket().getAddress() << "] " <<
            "Incoming for " << socket().getLocalAddress() << Epyx::log::endl;
    }
    ~TestNetSelectSocket()
    {
         Epyx::log::debug << "[" << socket().getAddress() << "] End" <<
             Epyx::log::endl;
    }

    void eat(const char *data, long size)
    {
        Epyx::log::debug << "[" << socket().getAddress() << "-RECV] " <<
            std::string(data, size) << Epyx::log::endl;
        socket().write("ACK");
    }
};

int main()
{
    Epyx::Thread::init();
    Epyx::log::init(Epyx::log::CONSOLE, "");
    Epyx::Socket::init();
    try {
        // Start server thread
        TestServerThread srvThread(4242);
        srvThread.setName("ServerThread");
        Epyx::log::debug << "Start " << srvThread.getThisName() << " at " <<
            srvThread.getAddress() << Epyx::log::endl;
        srvThread.start();

        // Start select() thread
        Epyx::NetSelect selectThread(20, "SelectWorker");
        // Do NOT delete tcpServer : it is deleted by NetSelectTCPServer
        Epyx::TCPServer *tcpServer = new Epyx::TCPServer(4243, selectThread.getNumWorkers());
        Epyx::log::debug << "ServerWorker listening at " << tcpServer->getAddress() <<
            Epyx::log::endl;
        selectThread.add(new Epyx::NetSelectTCPServer<TestNetSelectSocket>(tcpServer));
        selectThread.setName("NetSelect");
        Epyx::log::debug << "Start " << selectThread.getThisName() <<
            Epyx::log::endl;
        selectThread.start();

        // Wait threads
        srvThread.wait();
        selectThread.wait();
        Epyx::log::debug << "Server thread has terminated" << Epyx::log::endl;
    } catch (Epyx::Exception e) {
        Epyx::log::error << e << Epyx::log::endl;
    }
    Epyx::log::flushAndQuit();
}

