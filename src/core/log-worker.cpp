#include "log-worker.h"

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
        LogEntry entry = {message, prio, time(NULL), Thread::getName(), Thread::getId()};

        this->entries.push(entry);
    }

    void Worker::run() {
        //As this function is called from only one thread I can use static variables to avoid allocations
        static char time_buffer[20];
        static char date_buffer[20];

	    while(1){
            //Take all the elements from the queue with only one call
            std::deque<LogEntry>* entries = this->entries.flush();
            if(entries == NULL) continue;

            while(!entries->empty()){
                LogEntry& entry = entries->front();

                //This returns a pointer to a static struct, I hope I'm the only one using it
                //TODO: fix this!
                tm* timeinfo = localtime(&entry.time);
                strftime(time_buffer, 20, "%H:%M:%S", timeinfo);
                strftime(date_buffer, 20, "%Y-%m-%d", timeinfo);

                //Make the part with the thread's name
                std::ostringstream thread_buffer;
                int id = entry.thread_id;
                if(id < 0){
                    thread_buffer << "[" << entry.thread_name << "]";
                }else{
                    thread_buffer << "[" << entry.thread_name << " " << id << "]";
                }

                //TODO: Flush control commands
                //Do the actual IO
                if(this->flags & CONSOLE) {
                    std::cout << "[" << time_buffer << "] " << thread_buffer.str() << " " << entry.str << "\n";
                }
                if(this->flags & ERRORCONSOLE) {
                    std::cerr << "[" << time_buffer << "] " << thread_buffer.str() << " " << entry.str << "\n";
                }
                if(this->flags & LOGFILE) {
                    logFile << "[" << date_buffer << " " << time_buffer << "] " << thread_buffer.str() << " " << entry.str << "\n";
                }

                entries->pop_front();
            }
        }
    }

}
}
