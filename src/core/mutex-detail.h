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
 * @file mutex-detail.h
 * @brief a mutex implementation
 *
 * You should never include this file directly
 */
#ifndef EPYX_MUTEX_DETAIL_H
#define EPYX_MUTEX_DETAIL_H

#include "assert.h"
#include "exception.h"

namespace Epyx
{

    inline Mutex::Mutex() {
        int mutex_init_status = pthread_mutex_init(&(this->mutex), NULL);
        EPYX_ASSERT_NO_LOG(mutex_init_status == 0);
    }

    inline Mutex::~Mutex() {
        int mutex_destroy_status = pthread_mutex_destroy(&(this->mutex));
        EPYX_ASSERT_NO_LOG(mutex_destroy_status == 0);
    }

    inline void Mutex::lock() {
        int mutex_lock_status = pthread_mutex_lock(&(this->mutex));
        EPYX_ASSERT_NO_LOG(mutex_lock_status != EAGAIN);
        EPYX_ASSERT_NO_LOG(mutex_lock_status != EINVAL);
        EPYX_ASSERT_NO_LOG(mutex_lock_status == 0);
    }

    inline void Mutex::unlock() {
        int mutex_unlock_status = pthread_mutex_unlock(&(this->mutex));
        EPYX_ASSERT_NO_LOG(mutex_unlock_status == 0);
    }

    inline bool Mutex::tryLock() {
        int mutex_trylock_status = pthread_mutex_trylock(&(this->mutex));
        EPYX_ASSERT_NO_LOG(mutex_trylock_status == 0 || mutex_trylock_status == EBUSY);
        return mutex_trylock_status != EBUSY;
    }

    inline pthread_mutex_t* Mutex::getInternal() {
        return &mutex;
    }
}

#endif /* EPYX_MUTEX_DETAIL_H */
