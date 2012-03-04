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
        static inline std::string& ltrim(std::string& str) {
            str.erase(0, str.find_first_not_of(spaces));
            return str;
        }
        /**
         * @brief Remove right spaces
         *
         * @return The input parameter, without spaces on its right
         */
        static inline std::string& rtrim(std::string& str) {
            str.erase(str.find_last_not_of(spaces) + 1);
            return str;
        }
        /**
         * @brief Remove spaces on both sides of the string
         *
         * @return The input parameter, without spaces on its bounds
         */
        static inline std::string& trim(std::string& str) {
            return ltrim(rtrim(str));
        }

        /**
         * @brief Convert a string to an integer
         * @param str
         * @return (int)str
         */
        static long toInt(const char *str);
    };
}

#endif /* EPYX_STRING_H */
