/**
 */
#ifndef EPYX_LOG_WORKER_H
#define EPYX_LOG_WORKER_H

#include <fstream>
//#include <sstream>
#include <string>
#include "thread.h"
#include "blocking-queue.h"
#include "log.h"

namespace Epyx {
namespace log {

    class Worker {
<<<<<<< HEAD
    private:
=======
    private:            
>>>>>>> 5d0ccf16e5f21f57716bc24afb91340f930576aa
        Worker(const Worker&);
        const Worker& operator=(const Worker&);

        struct LogEntry {
            std::string str;
            int prio;
            tm time;
        };

        BlockingQueue<LogEntry> entries;
        int flags;
        std::ofstream logFile;
        Thread thread;

    public:
        Worker(int flags, const std::string& file);
        void write(const std::string& message, int prio);

        void run();
    };

}
}

#endif /* EPYX_LOG_WORKER_H */
