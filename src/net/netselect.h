/**
 * @file netselect.h
 * @brief Implement select() class to wait data on multiple net sockets
 */

#ifndef EPYX_NETSELECT_H
#define EPYX_NETSELECT_H

#include "../core/common.h"
#include "../core/atom/counter.h"
#include "../core/atom/map.h"
#include "netselectreader.h"

namespace Epyx
{
    class NetSelect;
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
        int add(NetSelectReader *nsr);

        /**
         * @brief Kill a reader
         * @param id
         */
        void kill(int id);

        /**
         * @brief Get the number of workers
         * @return the current number of workers
         */
        int getNumWorkers() const;

        /**
         * @brief Set the number of workers
         * @param n the number of workers needed
         */
        void setNumWorkers(int n);

    protected:
        void run();

    private:
        /**
         * @brief NetSelectReader information
         */
        struct NetSelectReaderInfo {
            // Reader
            NetSelectReader* reader;

            // It is alive
            bool alive;

            // It has data to be read (it is in the blocking queue)
            bool inQueue;

            NetSelectReaderInfo(NetSelectReader *nsr);
            ~NetSelectReaderInfo();
        };

        atom::Counter readersId;
        atom::Map<int, NetSelectReaderInfo*> readers;
        class Workers : public WorkerPool<int>
        {
        public:
            Workers(NetSelect *owner);
            void treat(int *nsriId);
        private:
            NetSelect *owner;
        } workers;

        friend void Workers::treat(int *nsriId);

        bool running;
    };
}
#endif /* EPYX_NETSELECT_H */
