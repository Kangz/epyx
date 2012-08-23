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
 * @file mutex.h
 * @brief mutex definition.
 */
#ifndef EPYX_MUTEX_H
#define EPYX_MUTEX_H

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <errno.h>

namespace Epyx
{
    /**
     * @class Mutex
     *
     * @brief A mutual exclusion lock "Mutex"
     *
     * This class is simply a wrapper around pthreads' mutex object to ease its use.
     */
    class Mutex : private boost::noncopyable
    {
    public:
        /**
         * @brief The Mutex constructor
         */
        Mutex();

        /**
         * @brief The Mutex destructor
         */
        ~Mutex();

        /**
         * @brief locks the Mutex
         */
        void lock();

        /**
         * @brief unlocks the previously locked Mutex
         */
        void unlock();

        /**
         * @brief tries to lock the Mutex
         * @return true if the Mutex was successfully locked, false otherwise
         */
        bool tryLock();

        /**
         * @brief an accessor to pthreads' internal mutex representation
         * @return this representation
         */
        pthread_mutex_t* getInternal();

    private:
        pthread_mutex_t mutex;
    };
}

#include "mutex-detail.h"

#endif /* EPYX_MUTEX_H */
