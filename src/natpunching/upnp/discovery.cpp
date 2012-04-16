#include "discovery.h"
#include <sstream>
#include "../../core/common.h"
#include "../../net/udpsocket.h"

namespace Epyx
{
    namespace UPNP
    {

        Discovery::Discovery()
        :NetSelectSocket(new UDPSocket(Address("239.255.255.250", 1900))),
        hasAnswer(false) {
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << String::crlf
                << "HOST: 239.255.255.250:1900" << String::crlf
                << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << String::crlf
                << "MAN: \"ssdp:discover\"" << String::crlf
                << "MX: 2" << String::crlf << String::crlf;
            socket().write(message.str());
        }

        void Discovery::eat(const char *data, long size) {
            int n;
            std::string error;
            log::debug << "Eating : " << size << " bytes" << log::endl;
            log::debug << data << log::endl;

            // Eat data with HTTP parser
            htpars.eat(data, size);
            GTTPacket *pkt = htpars.getPacket();
            if (pkt == NULL && htpars.getError(error)) {
                log::error << "HTTP Parser error during discovery:\n"
                    << error << log::endl;
            }

            // Parse a full HTTP packet
            if ((pkt != NULL) && (pkt->headers["usn"]).find("InternetGateway")) {
                std::string location = pkt->headers["location"];
                if ((n = location.find("http://")) != std::string::npos) {
                    location = location.substr(n + 7);
                }
                log::debug << location << log::endl;
                n = location.find('/');
                log::debug << location.substr(0, n) << log::endl;
                addr = Address(location.substr(0, n));
                rootPath = location.substr(n);
                htpars.reset();
                log::debug << "Address : " << addr << log::endl << "Path : " << rootPath << log::endl;
                log::flush();
                hasAnswer = true;
                answerCond.notifyAll();
            }
        }

        bool Discovery::waitAnswer(int timeout) {
            if (!hasAnswer)
                answerCond.timedWait(timeout, 0);
            return hasAnswer;
        }

        Address Discovery::getAddress() const {
            return addr;
        }

        std::string Discovery::getPath() const {
            return rootPath;
        }
    } // namespace UPNP
} // namespace Epyx
