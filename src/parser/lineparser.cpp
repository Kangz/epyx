#include "lineparser.h"
#include "../core/common.h"
#include <string.h>

namespace Epyx
{

    LineParser::LineParser()
    :buffer(NULL), bufsize(0) {
    }

    LineParser::~LineParser() {
        this->reset();
    }

    void LineParser::reset() {
        if (buffer != NULL) {
            delete[] buffer;
            buffer = NULL;
            bufsize = 0;
        }
        EPYX_ASSERT(bufsize == 0);
    }

    void LineParser::push(const char* data, long size) {
        EPYX_ASSERT(data != NULL && size > 0);

        // Append data to inernal buffer
        if (buffer != NULL) {
            char *tmp = new char[bufsize + size + 1];
            memcpy(tmp, buffer, bufsize);
            memcpy(tmp + bufsize, data, size);
            delete[] buffer;
            buffer = tmp;
            bufsize += size;
        } else {
            buffer = new char[size + 1];
            bufsize = size;
            memcpy(buffer, data, size);
        }
        // Add a zero at the end to be sure string terminates
        buffer[bufsize] = 0;
    }

    bool LineParser::popLine(std::string& line) {
        for (long i = 0; i < bufsize; i++) {
            if (buffer[i] == '\n') {
                // REplace end of line with a nul character
                if (i > 0 && buffer[i-1] == '\r')
                    buffer[i-1] = 0;
                else
                    buffer[i] = 0;
                line.assign(buffer);
                this->skip(i + 1);
                return true;
            }
        }
        return false;
    }

    bool LineParser::popData(char *data, long size) {
        EPYX_ASSERT(data != NULL && size > 0);

        // Not enough bytes in the buffer
        if (size > bufsize)
            return false;

        // Read data
        memcpy(data, buffer, size);
        this->skip(size);
        return true;
    }

    void LineParser::skip(long n) {
        EPYX_ASSERT(0 < n && n <= bufsize);
        if (n == bufsize) {
            delete[] buffer;
            buffer = NULL;
            bufsize = 0;
        } else {
            bufsize -= n;
            char *tmp = new char[bufsize + 1];
            memcpy(tmp, buffer + n, bufsize);
            delete[] buffer;
            buffer = tmp;
        }
    }
}
