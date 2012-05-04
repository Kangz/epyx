#include "counter.h"

namespace Epyx
{
    namespace atom
    {
        Counter::Counter(unsigned long initial)
        :cnt(initial) {
        }

        unsigned long Counter::get() {
            mut.lock();
            unsigned long val = cnt;
            mut.unlock();
            return val;
        }

        unsigned long Counter::getIncrement() {
            mut.lock();
            unsigned long val = cnt;
            cnt++;
            mut.unlock();
            return val;
        }
    }
}
