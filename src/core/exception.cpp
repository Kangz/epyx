#include "exception.h"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sstream>

namespace Epyx
{
    Exception::Exception(const char *type_, const char *module_,
                                 const char *message_)
        :type(type_), module(module_), message(message_)
    {
    }

    void Exception::append(const std::string& txt)
    {
        this->message += txt;
    }

    std::ostream& operator<<(std::ostream& os, Exception& e)
    {
        os << e.type << " in module " << e.module << ": " << e.message << "\n";
    }


    FailException::FailException(const char *module_, const char *message_)
        :Exception("Epyx fails", module_, message_)
    {
    }

    ErrException::ErrException(const char *module_, const char *fct)
        :FailException(module_, "")
    {
        std::stringstream msg;
        msg << fct << " error " << errno << ": " << strerror(errno);
        this->append(msg.str().c_str());
    }
}
