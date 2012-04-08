#include "gttpacket.h"

namespace Epyx
{
    GTTPacket::GTTPacket()
    :size(0), body(NULL)
    {
    }

    GTTPacket::~GTTPacket()
    {
        if (body)
            delete[] body;
    }

    char* GTTPacket::acquireBody(){
        char* temp = body;
        body = NULL;
        return temp;
    }

    std::ostream& operator<<(std::ostream& os, const GTTPacket& pkt)
    {
        os << "Proto " << pkt.protocol << " method " << pkt.method << std::endl;
        for (std::map<std::string, std::string>::const_iterator i = pkt.headers.begin();
             i != pkt.headers.end(); ++i) {
            os << (*i).first << ": " << (*i).second << std::endl;
        }
        if (pkt.size > 0) {
            os << "Content-length: " << pkt.size << std::endl;
            os << pkt.body;
        }
        return os;
    }
}
