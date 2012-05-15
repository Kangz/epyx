#include "string.h"
#include "cstring"
#include "assert.h"
#include "log.h"
#include <stdlib.h>
#include <errno.h>
#include <algorithm>

namespace Epyx
{
    const char String::spaces[] = " \t\n\v\f\r";
    const char String::crlf[] = "\r\n";

    std::string& String::ltrim(std::string& str) {
        str.erase(0, str.find_first_not_of(spaces));
        return str;
    }

    std::string& String::rtrim(std::string& str) {
        str.erase(str.find_last_not_of(spaces) + 1);
        return str;
    }

    std::string& String::trim(std::string& str) {
        return ltrim(rtrim(str));
    }

    std::string& String::toUpper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            std::ptr_fun<int, int>(std::toupper));
        return str;
    }

    std::string& String::toLower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            std::ptr_fun<int, int>(std::tolower));
        return str;
    }

    long String::toInt(const std::string& s) {
        return String::toInt(s.c_str());
    }

    long String::toInt(const char* str) {
        char *endptr = NULL;
        long l = strtol(str, &endptr, 10);
        EPYX_ASSERT(endptr != NULL);
        if (*endptr == '\0')
            return l;

        // An error happened
        if (endptr == str && l == 0)
            throw ParserException("String::toInt", "strtol");
        throw ParserException("String::toInt", "Invalid characters");
    }

    std::string String::fromInt(int n) {
        std::stringstream out;
        out << n;
        return out.str();
    }

    std::string String::fromUnsignedLong(unsigned long n) {
        std::stringstream out;
        out << n;
        return out.str();
    }

    char* String::toNewChar(const std::string& s) {
        char* res = new char[s.length()];
        memcpy(res, s.c_str(), s.length());
        return res;
    }
}
