#include "log.h"
#include <string>

namespace Epyx
{
namespace log
{

    Worker::Worker(int flags, const std::string& file)
    :Thread("Logging Worker"), flags(flags) {
        if (!file.empty() && (this->flags & LOGFILE)) {
            //TODO: Close it
            this->logFile.open(file.c_str());
            EPYX_ASSERT_NO_LOG(logFile.is_open())
        }
        // start thread
        this->start();
    }

    void Worker::write(const std::string& message, int prio) {
        LogEntry initializer = {message, prio, time(NULL), Thread::getName(), NULL};
        LogEntry* entry = new LogEntry(initializer);

        this->entries.push(entry);
    }

    void Worker::flush(bool wait) {
        LogEntry initializer = {"", FLUSH, 0, "", NULL};
        LogEntry* entry = new LogEntry(initializer);
        Condition* cond = NULL;

        if (wait) {
            entry->cond = new Condition();
            entry->cond->lock();
            cond = entry->cond;
        }

        this->entries.push(entry);

        //The entry will get destroyed but not the condition
        if (wait) {
            cond->wait();
            cond->unlock();
            delete cond;
        }
    }

    void Worker::quit() {
        LogEntry initializer = {"", QUIT, 0, "", NULL};
        LogEntry* entry = new LogEntry(initializer);

        this->entries.push(entry);

        this->wait();
    }

    std::string logLevelNames[5] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };

    void Worker::run() {
        while (1) {
            //Take all the elements from the queue with only one call
            std::deque<LogEntry*>* entries = this->entries.flush();
            if (entries == NULL) continue;

            while (!entries->empty()) {
                LogEntry* entry = entries->front();

                if (entry->prio == FLUSH || entry->prio == QUIT) {
                    if (this->flags & CONSOLE) std::cout << std::flush;
                    if (this->flags & ERRORCONSOLE) std::cerr << std::flush;
                    if (this->flags & LOGFILE) logFile << std::flush;

                    if (entry->cond != NULL) {
                        entry->cond->notify();
                    }

                    if (entry->prio == QUIT) {
                        //TODO clean up
                        return;
                    }
                } else {
                    this->printEntry(entry);
                }

                delete entry;

                entries->pop_front();
            }
        }
    }

    void Worker::printEntry(LogEntry* entry) {
        //As this function is called from only one thread I can use static variables to avoid allocations
        static char time_buffer[20];
        static char date_buffer[20];

        //This returns a pointer to a static struct, I hope I'm the only one using it
        //TODO: fix this!
        tm* timeinfo = localtime(&entry->time);
        strftime(time_buffer, 20, "%H:%M:%S", timeinfo);
        strftime(date_buffer, 20, "%Y-%m-%d", timeinfo);

        //Make the part with the thread's name
        std::ostringstream info_buffer;
        info_buffer << "[" << logLevelNames[entry->prio] << "] ";
        info_buffer << "[" << entry->thread_name << "]";

        //TODO: Flush control commands
        //Do the actual IO
        if (this->flags & CONSOLE) {
            std::cout << "[" << time_buffer << "] " << info_buffer.str() << " " << entry->str << "\n";
        }
        if (this->flags & ERRORCONSOLE) {
            std::cerr << "[" << time_buffer << "] " << info_buffer.str() << " " << entry->str << "\n";
        }
        if (this->flags & LOGFILE) {
            logFile << "[" << date_buffer << " " << time_buffer << "] " << info_buffer.str() << " " << entry->str << "\n";
        }
    }

}
}
