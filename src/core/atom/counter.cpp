#include "counter.h"

namespace Epyx
{
    namespace atom
    {
        Counter::Counter(unsigned int initial)
        :cnt(initial) {
        }

        unsigned int Counter::get() {
            mut.lock();
            unsigned int val = cnt;
            mut.unlock();
            return val;
        }

        unsigned int Counter::getIncrement() {
            mut.lock();
            unsigned int val = cnt;
            cnt++;
            mut.unlock();
            return val;
        }
    }
}
