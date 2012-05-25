/**
 * @file netselectreader.h
 * @brief object on which select () is ran
 */

#ifndef EPYX_NETSELECTREADER_H
#define EPYX_NETSELECTREADER_H

namespace Epyx
{
    class NetSelect;

    /**
     * @class NetSelectReader
     *
     * @brief Container used by NetSelect to remember watched sockets
     */
    class NetSelectReader
    {
    public:
        NetSelectReader();
        virtual ~NetSelectReader();

        /**
         * @brief Get the file descriptor of the socket which is select()'ed
         * @return socket_fd, or -1 if it is closed
         */
        virtual int getFileDescriptor() const = 0;

        /**
         * @brief Read everything it can read and returns
         * @return true on success, false on error or end of file
         */
        virtual bool read() = 0;

        /**
         * @brief Get the NetSelect objects which owns this object
         */
        NetSelect* getOwner() const;

        /**
         * @brief Set owner, only once
         */
        void setOwner(NetSelect *ns);
    private:
        // Disable copy
        NetSelectReader(const NetSelectReader&);
        const NetSelectReader& operator=(const NetSelectReader&);

        // NetSelect which owns this reader
        NetSelect *owner;
    };
}
#endif /* EPYX_NETSELECTREADER_H */
