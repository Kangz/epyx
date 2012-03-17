#include "netselect.h"
#include <sys/select.h>

namespace Epyx
{
    NetSelect::NetSelect(int numworkers, const std::string name)
        :running(true)
    {
        workers.setName(name);
        workers.setNumWorkers(numworkers);
    }

    NetSelect::~NetSelect()
    {
        // Stop working pool
        workers.stop();

        // Delete every selected stuff
        readersMutex.lock();
        for (std::map<NetSelectReader*, bool>::iterator i = readers.begin();
             i != readers.end(); i++) {
            if (i->first != NULL)
                delete(i->first);
        }
        readers.clear();
        readersMutex.unlock();
    }

    void NetSelect::add(NetSelectReader &nsr)
    {
        nsr.setOwner(this);
        readersMutex.lock();
        readers[&nsr] = false;
        readersMutex.unlock();
    }

    void NetSelect::run()
    {
        fd_set rfds;
        struct timeval tv;
        int fdmax;
        while (running) {
            // Build select parameters
            FD_ZERO(&rfds);
            fdmax = 0;
            readersMutex.lock();
            for (std::map<NetSelectReader*, bool>::iterator i = readers.begin();
                 i != readers.end(); i++) {
                if (!i->second) {
                    NetSelectReader *nsr = i->first;
                    EPYX_ASSERT(nsr != NULL);
                    int fd = nsr->getFileDescriptor();
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
            for (std::map<NetSelectReader*, bool>::iterator i = readers.begin();
                 i != readers.end(); i++) {
                NetSelectReader *nsr = i->first;
                EPYX_ASSERT(nsr != NULL);
                int fd = nsr->getFileDescriptor();
                if (!i->second && FD_ISSET(fd, &rfds))
                {
                    i->second = true;
                    workers.post(*nsr);
                }
            }
            readersMutex.unlock();
        }
    }

    void NetSelect::Workers::treat(NetSelectReader& nsr)
    {
        NetSelect *ns = nsr.getOwner();
        ns->readersMutex.lock();
        ns->readers[&nsr] = false;
        ns->readersMutex.unlock();

        if (!nsr.read()) {
            // Destroy this reader
            ns->readersMutex.lock();
            ns->readers.erase(&nsr);
            ns->readersMutex.unlock();
            delete &nsr;
        }
    }
}
