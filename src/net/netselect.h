/**
 * @file netselect.h
 * @brief Implement select() class to wait data on multiple net sockets
 */

#ifndef EPYX_NETSELECT_H
#define EPYX_NETSELECT_H

#include "../core/common.h"
#include "netselectreader.h"
#include <map>

namespace Epyx
{
    class NetSelect : public Thread
    {
    public:
        NetSelect(int numworkers, const std::string workerName);
        ~NetSelect();

        /**
         * @brief add a new reader
         *
         * After this call, nsr belongs to this NetSelect and shoulf no longer
         * be used by the caller. Moreover, nsr needs to have been created with
         * new (because it is destroyed by delete)
         */
        void add(NetSelectReader *nsr);

         /**
         * @brief gets the number of workers
         * @return the current number of workers
         */
        inline int getNumWorkers()
        {
            return workers.getNumWorkers();
        }

        /**
         * @brief sets the number of workers
         * @param n the number of workers needed
         */
        void setNumWorkers(int n)
        {
            workers.setNumWorkers(n);
        }

    protected:
        void run();

    private:
        // For each reader, tell wether it is already in the blocking queue
        // waiting to be read, to prevent useless select
        std::map<NetSelectReader*, bool> readers;
        Mutex readersMutex;

        class Workers : public WorkerPool<NetSelectReader>
        {
        public:
            inline Workers() : WorkerPool(false) {}
            void treat(NetSelectReader *nsr);
        } workers;

        friend void Workers::treat(NetSelectReader *nsr);

        bool running;
    };
}
#endif /* EPYX_NETSELECT_H */
