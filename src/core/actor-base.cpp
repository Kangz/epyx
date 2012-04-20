/**
 * @file actor-base.cpp
 * @brief Implementation of the base class of the actors
 */

#include "actor-base.h"

namespace Epyx
{

    Actor_base::Actor_base() {
        alive = true;
    }

    Actor_base::~Actor_base() {
    }

    void Actor_base::timeout() {
    }

    void Actor_base::lock() {
        this->mutex.lock();
    }

    void Actor_base::unlock() {
        this->mutex.unlock();
    }

}
