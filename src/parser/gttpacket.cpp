#include "gttpacket.h"
#include "../core/common.h"
#include <sstream>
#include <string.h>

namespace Epyx
{

    GTTPacket::GTTPacket()
    :size(0), body(NULL) {
    }

    GTTPacket::~GTTPacket() {
        if (body)
            delete[] body;
    }

    std::ostream& operator<<(std::ostream& os, const GTTPacket& pkt) {
        os << "[Proto " << pkt.protocol << " method " << pkt.method << std::endl;
        for (std::map<std::string, std::string>::const_iterator i = pkt.headers.begin();
            i != pkt.headers.end(); ++i) {
            os << "  " << (*i).first << ": " << (*i).second << std::endl;
        }
        if (pkt.size > 0) {
            os << "  content-length: " << pkt.size << std::endl;
        }
        os << "]";
        return os;
    }

    unsigned long GTTPacket::build(char **newData) const {
        EPYX_ASSERT(newData != NULL);
        std::stringstream head;
        std::map<std::string, std::string>::const_iterator it;

        // First line
        head << protocol << " " << method << String::crlf;

        // Headers
        for (it = headers.begin(); it != headers.end(); it++) {
            head << it->first << ": " << it->second << String::crlf;
        }
        if (size > 0) {
            head << "content-length: " << size << String::crlf;
        }
        head << String::crlf;

        // Concatenate headers and content
        const std::string str = head.str();
        unsigned long strSize = str.length();
        char *data = new char[strSize + size];
        strncpy(data, str.c_str(), strSize);
        memcpy(data + strSize, body, size);
        *newData = data;
        return strSize + size;
    }
}
