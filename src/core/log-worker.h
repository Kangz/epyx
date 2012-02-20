/**
 */
#ifndef EPYX_LOG_WORKER_H
#define EPYX_LOG_WORKER_H

#include <fstream>
//#include <sstream>
#include <string>
#include "thread.h"
#include "blocking-queue.h"
#include "condition.h"
#include "log.h"

namespace Epyx {
namespace log {

    class Worker : public Runnable {
    private:
        Worker(const Worker&);
        const Worker& operator=(const Worker&);

        struct LogEntry {
            std::string str;
            int prio;
            time_t time;
            std::string thread_name;
            int thread_id;
            Condition* cond;
        };

        BlockingQueue<LogEntry> entries;
        int flags;
        std::ofstream logFile;
        Thread thread;

        void printEntry(LogEntry* entry);

    public:
        Worker(int flags, const std::string& file);
        void write(const std::string& message, int prio);
        void flush(bool wait);
        void quit();

        virtual void run();
    };

}
}

#endif /* EPYX_LOG_WORKER_H */
