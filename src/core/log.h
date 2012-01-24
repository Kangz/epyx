/**
 */
#ifndef EPYX_LOG_H
#define EPYX_LOG_H

#include <fstream>
#include <sstream>
#include <string>
#include "log-worker.h"
#include "tls-pointer.h"

namespace Epyx {
namespace log {

    //Other modules should not access these variable
    //Here is the worker thread
    class Worker;
    extern Worker* _worker;

    //And this is the ofstream of each stream
    struct BufferContainer {
        std::ostringstream b[5];
    };
    extern TLSPointer<BufferContainer>* _buffers;

    //Debug levels
    enum {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    //Defines a Stream class to handle << operators nicely
    //This Struct is the endLog struct that ends the log line (a log line can be multiple lines)
    struct EndlStruct{};
    extern EndlStruct endl;

    class Stream {
    private:
        std::ostringstream buffer;
        int priority;

        Stream(const Stream&);
        const Stream& operator=(const Stream&);

    public:
        Stream(int prio);
        ~Stream();
        template<typename T> Stream& operator<<(const T& arg) {
            _buffers->get()->b[this->priority] << arg;
            return *this;
        }
        Stream& operator<<(const EndlStruct& f);
    };

    //Here the definition of the different log streams
    static Stream debug(DEBUG);
    static Stream info(INFO);
    static Stream warn(WARN);
    static Stream error(ERROR);
    static Stream fatal(FATAL);

    //Entry point for the logger
    enum {
        CONSOLE = 1,
        ERRORCONSOLE = 2,
        LOGFILE = 4
    };

    void init(int flags, const std::string& file = "");

}
}

#endif /* EPYX_LOG_H */
