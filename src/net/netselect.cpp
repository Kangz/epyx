#include "netselect.h"
#include <sys/select.h>

namespace Epyx
{

    NetSelect::NetSelect(int numworkers, const std::string& workerName)
    :workers(this), running(true) {
        workers.setName(workerName);
        workers.setNumWorkers(numworkers);
    }

    NetSelect::~NetSelect() {
        // Stop working pool
        workers.stop();

        // Stop running thread
        running = false;

        // Save NetSelectReaders before destroying the map
        std::list<NetSelectReader*> nsrBackup;
        for (atom::Map < NetSelectReader*, bool>::iterator i = readers.beginLock();
            !readers.isEnd(i); i++) {
            NetSelectReader *nsr = i->first;
            //log::debug << "NetSelect: About to destroy " << nsr << log::endl;
            if (nsr != NULL)
                nsrBackup.push_back(nsr);
        }
        readers.endUnlock();

        // Delete every selected stuff
        readers.clearForEver();
        while (!nsrBackup.empty()) {
            NetSelectReader *nsr = nsrBackup.front();
            nsrBackup.pop_front();
            //log::debug << "NetSelect: Destroy " << nsr << log::endl;
            delete nsr;
        }
    }

    void NetSelect::add(NetSelectReader *nsr) {
        EPYX_ASSERT(nsr != NULL);
        nsr->setOwner(this);
        readers.set(nsr, false);
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
            for (atom::Map < NetSelectReader*, bool>::const_iterator i = readers.beginLock();
                running && !readers.isEnd(i); i++) {
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
            readers.endUnlock();
            if (!running)
                return;

            // Refresh every 0.01sec the FD list
            tv.tv_sec = 0;
            tv.tv_usec = 10000;
            int status = select(fdmax + 1, &rfds, NULL, NULL, &tv);
            if (status < -1)
                throw ErrException("NetSelect", "select");

            // Add each FD to the blocking queue
            for (atom::Map < NetSelectReader*, bool>::iterator i = readers.beginLock();
                running && !readers.isEnd(i); i++) {
                NetSelectReader *nsr = i->first;
                EPYX_ASSERT(nsr != NULL);
                int fd = nsr->getFileDescriptor();
                if (!i->second && FD_ISSET(fd, &rfds)) {
                    i->second = true;
                    workers.post(nsr);
                }
            }
            readers.endUnlock();
        }
    }

    NetSelect::Workers::Workers(NetSelect *owner)
    :WorkerPool(false), owner(owner) {
    }

    void NetSelect::Workers::treat(NetSelectReader *nsr) {
        EPYX_ASSERT(owner != NULL);
        EPYX_ASSERT(nsr != NULL);
        if (!nsr->read()) {
            // Destroy this reader on false return
            if (owner->readers.unset(nsr)) {
                //log::debug << "NetSelect: Closing " << (nsr) << log::endl;
                delete nsr;
            }
        }

        // Tell this reader is treated
        owner->readers.set(nsr, false);
    }
}
