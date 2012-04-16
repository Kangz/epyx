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

    /**
     * @class NetSelect
     *
     * @brief Implement select() on multiple sockets using a WorkerPool
     *
     * Use NetSelect.start() to start this thread
     */
    class NetSelect : public Thread
    {
    public:
        /**
         * @brief Constructor
         * @param numworkers Initial workers number
         * @param workerName Name of workers
         */
        NetSelect(int numworkers, const std::string& workerName);
        /**
         * @brief Desctructor
         */
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
        int getNumWorkers() const;

        /**
         * @brief sets the number of workers
         * @param n the number of workers needed
         */
        void setNumWorkers(int n);

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
            inline Workers() : WorkerPool(false) {
            }
            void treat(NetSelectReader *nsr);
        } workers;

        friend void Workers::treat(NetSelectReader *nsr);

        bool running;
    };
}
#endif /* EPYX_NETSELECT_H */
