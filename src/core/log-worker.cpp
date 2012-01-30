#include "log-worker.h"
#include <string>

namespace Epyx {
namespace log {

    Worker::Worker(int flags_, const std::string& file): flags(flags_), thread(this, "Logging Worker"){
        if(!file.empty() && (this->flags & LOGFILE)) {
            //TODO: Close it
            this->logFile.open(file.c_str());

            if(!logFile.is_open()) {
                this->flags ^= LOGFILE;
                //How will FailException warn ? it should not use the logger
                //FailException("Logger", "Could not open the log file");
            }
        }
        this->thread.run();
    }

    void Worker::write(const std::string& message, int prio) {
        LogEntry entry = {message, prio, time(NULL), Thread::getName(), Thread::getId(), NULL};
        this->entries.push(entry);
    }

    void Worker::flush(bool wait) {
        LogEntry entry = {"", FLUSH, 0, "", 0, NULL};
        if(wait) {
            entry.cond = new Condition();
            entry.cond->lock();
        }

        this->entries.push(entry);

        if(wait) {
            entry.cond->wait();
            entry.cond->unlock();
            delete entry.cond;
        }
    }

    void Worker::quit(){
        LogEntry entry = {"", QUIT, 0, "", 0, NULL};

        this->entries.push(entry);

        this->thread.wait();
    }

    std::string logLevelNames[5] = {
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };


    void Worker::run() {
	    while(1){
            //Take all the elements from the queue with only one call
            std::deque<LogEntry>* entries = this->entries.flush();
            if(entries == NULL) continue;

            while(!entries->empty()){
                LogEntry& entry = entries->front();

                if(entry.prio == FLUSH || entry.prio == QUIT) {
                    if(this->flags & CONSOLE) std::cout << std::flush;
                    if(this->flags & ERRORCONSOLE) std::cerr << std::flush;
                    if(this->flags & LOGFILE) logFile << std::flush;

                    if(entry.cond != NULL) {
                        entry.cond->notify();
                    }

                    if(entry.prio == QUIT) {
                        //TODO clean up
                        return;
                    }
                } else {
                    this->printEntry(entry);
                }
                entries->pop_front();
            }
        }
    }

    void Worker::printEntry(LogEntry& entry) {
        //As this function is called from only one thread I can use static variables to avoid allocations
        static char time_buffer[20];
        static char date_buffer[20];

        //This returns a pointer to a static struct, I hope I'm the only one using it
        //TODO: fix this!
        tm* timeinfo = localtime(&entry.time);
        strftime(time_buffer, 20, "%H:%M:%S", timeinfo);
        strftime(date_buffer, 20, "%Y-%m-%d", timeinfo);

        //Make the part with the thread's name
        std::ostringstream info_buffer;
        int id = entry.thread_id;
        info_buffer << "[" << logLevelNames[entry.prio] << "] ";
        if(id < 0){
            info_buffer << "[" << entry.thread_name << "]";
        }else{
            info_buffer << "[" << entry.thread_name << " " << id << "]";
        }

        //TODO: Flush control commands
        //Do the actual IO
        if(this->flags & CONSOLE) {
            std::cout << "[" << time_buffer << "] " << info_buffer.str() << " " << entry.str << "\n";
        }
        if(this->flags & ERRORCONSOLE) {
            std::cerr << "[" << time_buffer << "] " << info_buffer.str() << " " << entry.str << "\n";
        }
        if(this->flags & LOGFILE) {
            logFile << "[" << date_buffer << " " << time_buffer << "] " << info_buffer.str() << " " << entry.str << "\n";
        }
    }

}
}
