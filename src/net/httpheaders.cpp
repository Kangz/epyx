#include "httpheaders.h"
#include <sstream>

namespace Epyx{
    std::string HTTPHeaders::getHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(0,pos);
    }
    std::string HTTPHeaders::stripHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(pos+4);
    }
    int HTTPHeaders::getlength(std::string header) {
        int deb = header.find("Content-Length: ") + 16;
        int fin = header.find("\r\n",deb);
        std::string len = header.substr(deb, fin - deb);
        int numb;
        std::istringstream ( len ) >> numb;
        return numb + header.length() + 4; // Num is the length of the HTTP packet. We have the header length to add, and the for characters "\r\n\r\n" that are stripped
    }

}
