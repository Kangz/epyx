/*
 *   Copyright 2012 Epyx Team
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
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
