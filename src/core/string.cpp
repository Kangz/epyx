#include "string.h"
#include "assert.h"
#include "log.h"
#include <stdlib.h>
#include <errno.h>

namespace Epyx {
    const char String::spaces[] = " \t\n\v\f\r";

    long String::toInt(const char *str)
    {
        char *endptr = NULL;
        long l = strtol(str, &endptr, 10);
        EPYX_ASSERT(endptr != NULL);
        if (*endptr == '\0')
            return l;

        // An error happened
        if (endptr == str && l == 0)
            throw new ErrException("String::toInt", "strtol");
        throw new FailException("String::toInt", "Invalid characters");
    }
}
