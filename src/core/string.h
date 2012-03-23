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
         * @brief Remove left spaces
         *
         * @return The input parameter, without spaces on its left
         */
        static std::string& ltrim(std::string& str);
        /**
         * @brief Remove right spaces
         *
         * @return The input parameter, without spaces on its right
         */
        static std::string& rtrim(std::string& str);
        /**
         * @brief Remove spaces on both sides of the string
         *
         * @return The input parameter, without spaces on its bounds
         */
        static std::string& trim(std::string& str);

        /**
         * @brief Transform a string to uppercase
         * @param str reference to a string
         * @return str
         */
        static std::string& toUpper(std::string& str);
        /**
         * @brief Transform a string to lowercase
         * @param str reference to a string
         * @return str
         */
        static std::string& toLower(std::string& str);

        /**
         * @brief Convert a string to an integer
         * @param str
         * @return (int)str
         */
        static long toInt(const char *str);
    };
}

#endif /* EPYX_STRING_H */
