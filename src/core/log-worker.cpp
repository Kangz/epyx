#include "log-worker.h"

namespace Epyx {
namespace log {

    static void start_worker(void* worker){
        ((Worker*)worker)->run();
    }

    Worker::Worker(int flags_, const std::string& file): flags(flags_), thread(start_worker, this){
        if(!file.empty() && (this->flags & LOGFILE)) {
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
        time_t rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        LogEntry entry = {message, prio, *timeinfo};

        this->entries.push(entry);
    }

    void Worker::run() {
	    while(1){
            //TODO use flush instead
            std::deque<LogEntry>* entries = this->entries.flush();
            if(entries == NULL) continue;
            while(!entries->empty()){
                LogEntry& entry = entries->front();
                if(this->flags & CONSOLE) {
                    std::cout << entry.str << "\n";
                }
                if(this->flags & ERRORCONSOLE) {
                    std::cerr << entry.str << "\n";
                }
                if(this->flags & LOGFILE) {
                    //TODO
                    // std::cout << entry.str << std::endl;
                }
                entries->pop_front();
            }
        }
    }

}
}
