/**
 * This program tests Server class
 */

#include <cstring>
#include "api.h"
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
     * @param unused
     */
    TestNetSelectSocket(std::shared_ptr<Epyx::Socket> sock, void *unused)
        :NetSelectSocket(sock)
    {
        Epyx::log::debug << "[" << socket()->getAddress() << "] " <<
            "Incoming for " << socket()->getLocalAddress() << Epyx::log::endl;

        // Motd
        std::ostringstream hello;
        hello << "Hello, you address is " << socket()->getAddress() << " !\n" <<
            "Type EXIT or QUIT to exit the session\n" <<
            "Type PAN to kill the session\n";
        socket()->write(hello.str());
    }
    /**
     * @brief End a connection
     */
    ~TestNetSelectSocket()
    {
         Epyx::log::debug << "[" << socket()->getAddress() << "] End" <<
             Epyx::log::endl;
    }

    /**
     * @brief Treat received data
     */
    void eat(const Epyx::byte_str& data)
    {
        std::string strline = bytes2string_c(data);
        Epyx::String::trim(strline);
        Epyx::log::debug << "[" << socket()->getAddress() << "-RECV] " <<
           strline << Epyx::log::endl;

        const char *line = strline.c_str();

        if (!strcasecmp(line, "quit") || !strcasecmp(line, "exit")) {
            // Close socket
            this->socket()->close();
        } else if (!strcasecmp(line, "pan")) {
            throw Epyx::MinorException("test-server", "A client tried to kill me");
        } else if (!strcasecmp(line, "o<")) {
            socket()->write("PAN !\n");
        } else {
            // Mirror
            socket()->write("Mirror: ");
            socket()->sendBytes(data);
        }
    }
};

int main()
{
    Epyx::API epyx;
    try {
        // Start select() thread
        std::unique_ptr<Epyx::NetSelect> netselect(new Epyx::NetSelect(20, "SelectWorker"));
        // Do NOT delete tcpServer : it is deleted by NetSelectTCPServer
        const Epyx::SockAddress addr("", 4242);
        Epyx::TCPServer *tcpServer = new Epyx::TCPServer(addr, 20);
        Epyx::log::debug << "ServerWorker listening at " << tcpServer->getAddress() <<
            Epyx::log::endl;
        std::shared_ptr<Epyx::NetSelectReader> srv(new Epyx::NetSelectTCPServer<TestNetSelectSocket, void*>(tcpServer, NULL));
        netselect->add(srv);
        Epyx::log::debug << "Start NetSelect" << Epyx::log::endl;
        std::thread selectThread(&Epyx::NetSelect::runLoop, netselect.get(), "NetSelect");

        // Wait thread
        selectThread.join();
        Epyx::log::debug << "Server thread has terminated" << Epyx::log::endl;
    } catch (Epyx::Exception e) {
        Epyx::log::fatal << e << Epyx::log::endl;
    }
    return 0;
}

