#include "discovery.h"
#include <sstream>
#include "../../core/common.h"
#include "../../net/udpsocket.h"

namespace Epyx
{
    namespace UPNP
    {

        Discovery::Discovery()
        :NetQuery(new UDPSocket(Address("239.255.255.250", 1900))) {
        }

        bool Discovery::query() {
            std::stringstream message;
            message << "M-SEARCH * HTTP/1.1" << String::crlf
                << "HOST: 239.255.255.250:1900" << String::crlf
                << "ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1" << String::crlf
                << "MAN: \"ssdp:discover\"" << String::crlf
                << "MX: 2" << String::crlf << String::crlf;
            return socket().write(message.str());
        }

        URI* Discovery::eat(const char *data, long size) {
            int n;
            std::string error;
            //log::debug << "Eating : " << size << " bytes" << log::endl;
            //log::debug << data << log::endl;

            // Eat data with HTTP parser
            htpars.eat(data, size);
            GTTPacket *pkt = htpars.getPacket();
            if (pkt == NULL) {
                // Incomplete packet, there may be an error
                if (htpars.getError(error)) {
                    log::error << "HTTP Parser error during discovery:\n"
                        << error << log::endl;
                }
                return NULL;
            }

            // Here, pkt is an HTTP packet.
            // Filter in Internet Gateways
            if (!pkt->headers["usn"].find("InternetGateway"))
                return NULL;

            // Return location
            return new URI(pkt->headers["location"]);
        }
    }
}
