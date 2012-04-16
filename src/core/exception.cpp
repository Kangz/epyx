#include "exception.h"
#include "log.h"
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sstream>

namespace Epyx
{

    Exception::Exception(const char *type, const char *module,
        const char *message)
    :type(type), module(module), message(message) {
    }

    void Exception::append(const std::string& txt) {
        message += txt;
    }

    std::ostream& operator<<(std::ostream& os, const Exception& e) {
        return os << e.type << " in module " << e.module << ": " << e.message << "\n";
    }

    FailException::FailException(const char *module, const char *message)
    :Exception("Epyx fails", module, message) {
    }

    ErrException::ErrException(const char *module, const char *fct)
    :FailException(module, "") {
        std::stringstream msg;
        log::error << fct << ": " << log::errstd << log::endl;
        msg << "Error with " << fct;
        this->append(msg.str().c_str());
    }
}
