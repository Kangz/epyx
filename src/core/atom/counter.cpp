#include "counter.h"

namespace Epyx
{
    namespace atom
    {
        Counter::Counter(unsigned long initial)
        :cnt(initial) {
        }

        unsigned long Counter::get() {
            std::lock_guard<std::mutex> lock(mut);
            unsigned long val = cnt;
            return val;
        }

        unsigned long Counter::getIncrement() {
            std::lock_guard<std::mutex> lock(mut);
            unsigned long val = cnt;
            cnt++;
            return val;
        }
    }
}
