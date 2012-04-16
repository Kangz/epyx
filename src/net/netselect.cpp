#include "netselect.h"
#include <sys/select.h>

namespace Epyx
{

    NetSelect::NetSelect(int numworkers, const std::string& workerName)
    :running(true) {
        workers.setName(workerName);
        workers.setNumWorkers(numworkers);
    }

    NetSelect::~NetSelect() {
        // Stop working pool
        workers.stop();

        // Stop running thread
        running = false;

        // Delete every selected stuff
        readersMutex.lock();
        for (std::map < NetSelectReader*, bool>::iterator i = readers.begin();
            i != readers.end(); i++) {
            if (i->first != NULL)
                delete(i->first);
        }
        readers.clear();
        readersMutex.unlock();
    }

    void NetSelect::add(NetSelectReader *nsr) {
        EPYX_ASSERT(nsr != NULL);
        nsr->setOwner(this);
        readersMutex.lock();
        readers[nsr] = false;
        readersMutex.unlock();
    }

    int NetSelect::getNumWorkers() const {
        return workers.getNumWorkers();
    }

    void NetSelect::setNumWorkers(int n) {
        workers.setNumWorkers(n);
    }

    void NetSelect::run() {
        fd_set rfds;
        struct timeval tv;
        int fdmax;
        while (running) {
            // Build select parameters
            FD_ZERO(&rfds);
            fdmax = 0;
            readersMutex.lock();
            for (std::map < NetSelectReader*, bool>::iterator i = readers.begin();
                i != readers.end(); i++) {
                if (!i->second) {
                    NetSelectReader *nsr = i->first;
                    EPYX_ASSERT(nsr != NULL);
                    int fd = nsr->getFileDescriptor();
                    EPYX_ASSERT(fd >= 0);
                    FD_SET(fd, &rfds);
                    if (fd > fdmax)
                        fdmax = fd;
                }
            }
            readersMutex.unlock();

            // Refresh every 0.01sec the FD list
            tv.tv_sec = 0;
            tv.tv_usec = 10000;
            int status = select(fdmax + 1, &rfds, NULL, NULL, &tv);
            if (status < -1)
                throw ErrException("NetSelect", "select");

            // Add each FD to the blocking queue
            readersMutex.lock();
            for (std::map < NetSelectReader*, bool>::iterator i = readers.begin();
                i != readers.end(); i++) {
                NetSelectReader *nsr = i->first;
                EPYX_ASSERT(nsr != NULL);
                int fd = nsr->getFileDescriptor();
                if (!i->second && FD_ISSET(fd, &rfds)) {
                    i->second = true;
                    workers.post(nsr);
                }
            }
            readersMutex.unlock();
        }
    }

    void NetSelect::Workers::treat(NetSelectReader *nsr) {
        // Tell this reader is treated
        NetSelect *nsel = nsr->getOwner();
        nsel->readersMutex.lock();
        nsel->readers[nsr] = false;
        nsel->readersMutex.unlock();

        if (!nsr->read()) {
            // Destroy this reader on false return
            nsel->readersMutex.lock();
            nsel->readers.erase(nsr);
            nsel->readersMutex.unlock();
            delete nsr;
        }
    }
}
