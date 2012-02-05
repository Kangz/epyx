#include "httpheaders.h"
namespace Epyx{
    HTTPHeaders::getHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(0,pos);
    }
    HTTPHeaders::stripHeaders(std::string message){
        int pos = message.find("\r\n\r\n");
        return message.substr(pos+4);
    }

}
