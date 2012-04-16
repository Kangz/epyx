/**
 * @file log-worker.h
 * @brief Used for the implementation of the logger
 */
#ifndef EPYX_LOG_WORKER_H
#define EPYX_LOG_WORKER_H

#include <fstream>
#include <string>
#include "thread.h"
#include "blocking-queue.h"
#include "condition.h"

namespace Epyx
{
namespace log
{
    class Worker : public Thread
    {
    public:
        Worker(int flags, const std::string& file);
        void write(const std::string& message, int prio);
        void flush(bool wait);
        void quit();

    protected:
        virtual void run();

    private:
        Worker(const Worker&);
        const Worker& operator=(const Worker&);

        struct LogEntry
        {
            std::string str;
            int prio;
            time_t time;
            std::string thread_name;
            Condition* cond;
        };

        BlockingQueue<LogEntry> entries;
        int flags;
        std::ofstream logFile;

        void printEntry(LogEntry* entry);

    };

}
}

#endif /* EPYX_LOG_WORKER_H */
