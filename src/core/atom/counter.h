/**
 * @file counter.h
 * @brief Atomic counter management
 */

#ifndef ATOMICCOUNTER_H
#define ATOMICCOUNTER_H

#include "../mutex.h"

namespace Epyx
{
    namespace atom
    {
        /**
         * @class Counter
         * @brief Atomic counter implementation
         */
        class Counter
        {
        public:
            /**
             * @brief Constructor
             * @param initial Initial value
             */
            Counter(unsigned int initial = 0);

            /**
             * @brief Get value
             * @return counter value
             */
            unsigned int get();

            /**
             * @brief Increment and get old value of the counter
             * @return value before increment
             */
            unsigned int getIncrement();

        private:
            Mutex mut;
            unsigned int cnt;

            // Disable copy
            Counter(const Counter&);
            const Counter& operator=(const Counter&);
        };
    }
}

#endif /* ATOMICCOUNTER_H */

