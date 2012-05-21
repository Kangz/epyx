/**
 * @file timeout.h
 * @brief Millisecond timeout
 */

#ifndef EPYX_TIMEOUT_H
#define EPYX_TIMEOUT_H

#include <sys/time.h>

namespace Epyx
{
    /**
     * @class Timeout
     * @brief Millisecond timeout implementation
     */
    class Timeout
    {
    public:
        /**
         * @brief Build a new timeout
         * @param ms Maximum number of milliseconds
         */
        Timeout(unsigned int ms);

        /**
         * @brief Has time run out ?
         * @return true if it is the case
         */
        bool hasExpired() const;

        /**
         * @brief Get remaining time in a timeval
         * @return struct timeval
         */
        struct timeval remainingTimeval() const;

        /**
         * @brief Get remaining time in msec
         * @return int msec
         */
        int remainingMsec() const;

        /**
        * @brief compares 2 timeout to know which one is the closest
        * @return true if t1's due date is before t2's
        */
        friend bool operator<(const Timeout& t1, const Timeout& t2);

    private:
        struct timeval maxTime;
    };
}

#endif /* EPYX_TIMEOUT_H */

