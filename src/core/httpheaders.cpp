#include "httpheaders.h"
namespace Epyx{
    std::string HTTPHeaders::getHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(0,pos);
    }
    std::string HTTPHeaders::stripHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(pos+4);
    }

}
