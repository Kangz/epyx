/**
 * @file gttparser.h
 * @brief Generic Text Template implementation
 */

#ifndef EPYX_GTTPARSER_H
#define EPYX_GTTPARSER_H

#include "gttpacket.h"
#include "lineparser.h"
#include <ctype.h>
#include <string>

namespace Epyx
{
    /**
     * @class GTTParser
     *
     * @brief GTT implementation
     */
    class GTTParser {
    public:
        /**
         * @brief Constructor
         */
        GTTParser();
        /**
         * @brief Destructor
         */
        ~GTTParser();

        /**
         * @brief reset internal state
         */
        void reset();

        /**
         * @brief get error, if any
         * @return true if there is an error
         */
        bool getError(std::string& userErr);

        /**
         * @brief eat some fresh data
         * @param data data to eat
         * @param size available size
         */
        void eat(const char *data, long size);

        /**
         * @brief get the last read packet
         * @return a packet or NULL. If not NULL, the caller is responsible of delete this packet
         */
        GTTPacket* getPacket();

    private:
        class ParserException
        {
        public:
            ParserException(const char *message)
            :message(message)
            {}
            const char *message;
        };
        GTTPacket *currentPkt;
        std::string errorMessage;
        bool hasError;

        // Use LineParser to parse lines
        LineParser lineParser;

        // Position in data
        enum DataPosition {
            dataposFirstLine,
            dataposHeaders,
            dataposContent
        } datapos;

        // First line
        std::string firstline;

        /**
         * @brief start a new packet, without cleaning read data
         */
        void startPacket();

        void setError(const char *msg);

        /**
         * @brief Parse the first line of GTT
         * @param line first line
         * @throw ParserException on errors
         */
        void parseFirstLine(const std::string& line);

        /**
         * @brief Parse a header line of GTT
         * @param line header line
         * @throw ParserException on errors
         */
        void parseHeaderLine(const std::string& line);

    };
}

#endif /* EPYX_GTTPARSER_H */

