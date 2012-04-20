/**
 * @file timeout.h
 * @brief Millisecond timeout
 */

#ifndef TIMEOUT_H
#define TIMEOUT_H

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
         * @param ms Maximum number of seonds
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

    private:
        struct timeval maxTime;
    };
}

#endif /* TIMEOUT_H */

