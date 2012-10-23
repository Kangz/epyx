/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
/**
 * @file counter.h
 * @brief Atomic counter management
 */

#ifndef EPYX_ATOM_COUNTER_H
#define EPYX_ATOM_COUNTER_H

#include <mutex>
#include <boost/noncopyable.hpp>

namespace Epyx
{
    namespace atom
    {
        /**
         * @class Counter
         * @brief Atomic counter implementation
         */
        class Counter : private boost::noncopyable
        {
        public:
            /**
             * @brief Constructor
             * @param initial Initial value
             */
            Counter(unsigned long initial = 0);

            /**
             * @brief Get value
             * @return counter value
             */
            unsigned long get();

            /**
             * @brief Increment and get old value of the counter
             * @return value before increment
             */
            unsigned long getIncrement();

        private:
            std::mutex mut;
            unsigned long cnt;
        };
    }
}

#endif /* EPYX_ATOM_COUNTER_H */

