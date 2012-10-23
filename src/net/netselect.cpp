#include "netselect.h"
#include <sys/select.h>

namespace Epyx
{

    NetSelect::NetSelect(int numworkers, const std::string& workerName)
    :readersId(1), workers(this), running(true) {
        workers.setName(workerName);
        workers.setNumWorkers(numworkers);
    }

    NetSelect::~NetSelect() {
        // Stop working pool
        workers.stop();

        // Stop running thread
        running = false;

        // Save NetSelectReaders before destroying the map
        std::list<NetSelectReaderInfo*> nsriBackup;
        for (atom::Map < int, NetSelectReaderInfo*>::iterator i = readers.beginLock();
            !readers.isEnd(i); i++) {
            NetSelectReaderInfo *nsri = i->second;
            //log::debug << "NetSelect: About to destroy " << nsr << log::endl;
            if (nsri != NULL)
                nsriBackup.push_back(nsri);
        }
        readers.endUnlock();

        // Delete every selected stuff
        readers.clearForEver();
        while (!nsriBackup.empty()) {
            NetSelectReaderInfo *nsri = nsriBackup.front();
            nsriBackup.pop_front();
            //log::debug << "NetSelect: Destroy " << nsr << log::endl;
            delete nsri;
        }
    }

    int NetSelect::add(NetSelectReader *nsr) {
        EPYX_ASSERT(nsr != NULL);
        nsr->setOwner(this);
        int id = readersId.getIncrement();
        readers.set(id, new NetSelectReaderInfo(nsr));
        return id;
    }

    void NetSelect::kill(int id) {
        NetSelectReaderInfo *nsri = readers.getAndLock(id, NULL);
        if (nsri != NULL) {
            nsri->alive = false;
            readers.endUnlock();
            nsri = readers.getUnset(id, NULL);
            if (nsri != NULL)
                delete nsri;
        } else {
            readers.endUnlock();
        }
    }

    NetSelectReader* NetSelect::get(int id) {
        NetSelectReaderInfo *info = readers.get(id, NULL);
        if (info == NULL)
            return NULL;
        return info->reader;
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
            for (atom::Map < int, NetSelectReaderInfo*>::const_iterator i = readers.beginLock();
                running && !readers.isEnd(i); i++) {
                NetSelectReaderInfo *nsri = i->second;
                EPYX_ASSERT(nsri != NULL);
                if (!nsri->alive || nsri->inQueue)
                    continue;
                EPYX_ASSERT(nsri->reader != NULL);
                int fd = nsri->reader->getFileDescriptor();
                if (fd >= 0) {
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
            std::list<int> closedList;
            for (atom::Map < int, NetSelectReaderInfo*>::iterator i = readers.beginLock();
                running && !readers.isEnd(i); i++) {
                NetSelectReaderInfo *nsri = i->second;
                EPYX_ASSERT(nsri != NULL);
                if (!nsri->alive)
                    continue;
                EPYX_ASSERT(nsri->reader != NULL);
                int fd = nsri->reader->getFileDescriptor();
                if (fd >= 0) {
                    if (!nsri->inQueue && FD_ISSET(fd, &rfds)) {
                        nsri->inQueue = true;
                        workers.post(new int(i->first));
                    }
                } else {
                    closedList.push_back(i->first);
                }
            }
            readers.endUnlock();

            // Remove closed sockets
            while (!closedList.empty()) {
                this->kill(closedList.front());
                closedList.pop_front();
            }
        }
    }

    NetSelect::NetSelectReaderInfo::NetSelectReaderInfo(NetSelectReader *nsr)
    :reader(nsr), alive(true), inQueue(false) {
    }

    NetSelect::NetSelectReaderInfo::~NetSelectReaderInfo() {
        if (reader != NULL) {
            delete reader;
            reader = NULL;
        }
    }

    NetSelect::Workers::Workers(NetSelect *owner)
    :owner(owner) {
    }

    void NetSelect::Workers::treat(std::unique_ptr<int> nsriId) {
        EPYX_ASSERT(owner != NULL);
        EPYX_ASSERT(nsriId);
        NetSelectReaderInfo *nsri = owner->readers.get(*nsriId, NULL);
        if (nsri == NULL || !nsri->alive)
            return;
        NetSelectReader *nsr = nsri->reader;
        EPYX_ASSERT(nsr != NULL);

        if (!nsr->read()) {
            // Destroy this reader on false return
            owner->kill(*nsriId);
            return;
        }
        // Tell this reader is treated
        nsri = owner->readers.getAndLock(*nsriId, NULL);
        if (nsri != NULL)
            nsri->inQueue = false;
        owner->readers.endUnlock();
    }
}
