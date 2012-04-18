/**
 * @file httpparser.h
 * @brief HTTP Parser
 */

#ifndef EPYX_HTTPPARSER_H
#define EPYX_HTTPPARSER_H

#include "gttparser.h"

namespace Epyx
{
    /**
     * @class HTTPParser
     *
     * @brief HTTP implementation
     */
    class HTTPParser : public GTTParser {
    private:
        /**
         * @brief Parse the first line of HTTP
         * @param line first line
         * @throw ParserException on errors
         */
        void parseFirstLine(const std::string& line);

        /**
         * @brief Parse a header line of HTTP
         * @param line header line
         * @throw ParserException on errors
         */
        void parseHeaderLine(const std::string& line);

    };
}

#endif /* EPYX_HTTPPARSER_H */

