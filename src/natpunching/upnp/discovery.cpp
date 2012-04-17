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
            if (pkt == NULL) {
                // Incomplete packet, there may be an error
                if (htpars.getError(error)) {
                    log::error << "HTTP Parser error during discovery:\n"
                        << error << log::endl;
                }
                return;
            }

            // Here, pkt is an HTTP packet.
            // Filter in Internet Gateways
            if (!pkt->headers["usn"].find("InternetGateway"))
                return;

            std::string location = pkt->headers["location"];
            discoveredURI = URI(location);
            htpars.reset();
            log::debug << "Location : " << location << log::endl;
            log::debug << "URI : " << discoveredURI << log::endl;

            // We have an answer !
            hasAnswer = true;
            answerCond.notifyAll();
        }

        bool Discovery::waitAnswer(int timeout) {
            if (!hasAnswer)
                answerCond.timedWait(timeout, 0);
            return hasAnswer;
        }

        const URI& Discovery::getURI() const {
            return discoveredURI;
        }
    } // namespace UPNP
} // namespace Epyx
