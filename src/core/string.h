/**
 * @file string.h
 * @brief Useful string subroutines
 */

#ifndef EPYX_STRING_H
#define EPYX_STRING_H

#include <ctype.h>
#include <string>

namespace Epyx
{
    /**
     * @class String
     *
     * @brief Useful string functions
     */
    class String {
    public:
        /**
         * @brief space characters
         */
        static const char spaces[];

        /**
         * @brief ASCII CR LF
         */
        static const char crlf[];

        /**
         * @brief Removes left spaces
         *
         * @return The input parameter, without spaces on its left
         */
        static std::string& ltrim(std::string& str);
        /**
         * @brief Removes right spaces
         *
         * @return The input parameter, without spaces on its right
         */
        static std::string& rtrim(std::string& str);
        /**
         * @brief Removes spaces on both sides of the string
         *
         * @return The input parameter, without spaces on its bounds
         */
        static std::string& trim(std::string& str);

        /**
         * @brief Transforms a string to uppercase
         * @param str reference to a string
         * @return str
         */
        static std::string& toUpper(std::string& str);
        /**
         * @brief Transforms a string to lowercase
         * @param str reference to a string
         * @return str
         */
        static std::string& toLower(std::string& str);

        /**
         * @brief Converts a string to an integer
         * @param str
         * @return (int)str
         * @throw ParserException if str is not an integer
         */
        static long toInt(const std::string& str);

       /**
         * @brief Converts a string to an integer
         * @param str
         * @return (int)str
         * @throw ParserException if str is not an integer
         */
        static long toInt(const char *str);

        /**
         * @brief Converts an integer to a string
         * @param n
         * @return (string)n
         */
        static std::string fromInt(int n);
        /**
         * @brief Converts an integer to a string
         * @param n
         * @return (string)n
         */
        static std::string fromUnsignedLong(unsigned long n);

        /**
         * @brief Returns a new copy of the content of the argument
         * @param str
         * @return A copy of the content of the argument
         */
        static char* toNewChar(const std::string& str);

    private:
        // Forbid instanciation
        String();
    };
}

#endif /* EPYX_STRING_H */
