/**
 * @file gttparser.h
 * @brief Generic Text Template implementation
 */

#ifndef EPYX_GTTPARSER_H
#define EPYX_GTTPARSER_H

#include "gttpacket.h"
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
        enum TokenType {
            start,
            protocol_name,
            method,
            method_line,
            flag_name,
            flag_indicator,
            flag_value,
            flag_line,
            newline,
            body,
            other
        };
        /**
         * @brief a Token is a string with a specfic type
         */
        struct Token
        {
            TokenType type;
            std::string str;
        };

        GTTParser();
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
        char *reminded;
        long remindedSize;
        std::string errorMessage;
        bool hasError;
        TokenType currentType;
        std::string currentString;
        TokenType flagType;
        std::string currentFlagName;
        long bodyCounter; // Internal counter

        /**
         * @brief start a new packet, without cleaning read data
         */
        void startPacket();

        void setError(const char *msg);

        bool processChar(char c);

    };
}

#endif /* EPYX_GTTPARSER_H */

