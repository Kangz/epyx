/**
 * @file lineparser.h
 * @brief Parse a stream line by line
 */

#ifndef EPYX_LINEPARSER_H
#define EPYX_LINEPARSER_H

#include <string>

namespace Epyx
{

    /**
     * @class LineParser
     *
     * @brief Cut raw data into lines
     */
    class LineParser
    {
    public:
        /**
         * @brief Constructor
         */
        LineParser();
        /**
         * @brief Destructor
         */
        ~LineParser();

        /**
         * @brief Reset everything
         */
        void reset();

        /**
         * @brief Push raw data into this object
         * @param data pushed data
         * @param size data size
         */
        void push(const char *data, long size);

        /**
         * @brief Pop one line
         * @param line string buffer where the line is read
         * @return true if a complete line was read
         */
        bool popLine(std::string& line);

        /**
         * @brief Pop raw data
         * @param data data to be written
         * @param size data size
         * @return true if there is enough internal data
         */
        bool popData(char *data, long size);

    private:
        /**
         * @brief Skip some data bytes
         * @param n number of bytes to discard
         */
        void skip(long n);

        char *buffer;
        long bufsize;
    };
}

#endif /* EPYX_LINEPARSER_H */
