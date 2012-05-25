/**
 * @file address.h
 * @brief IP addresses and ports management
 */
#ifndef EPYX_ADDRESS_H
#define EPYX_ADDRESS_H

#include <iostream>
#include <string>
#include <arpa/inet.h>

namespace Epyx
{
    /**
     * @class Address
     *
     * @brief Manage IP:port association
     *
     * This class manage both IPv4 et IPv6 address
     */
    class Address
    {
    public:
        /**
         * @brief Empty constructor
         */
        Address();
        /**
         * @brief Create an address with IP and port
         * @param ip IP address
         * @param port TCP or UDP port
         * @param ipVersion 4 or 6
         */
        Address(const std::string& ip, unsigned short port, int ipVersion = 4);
        /**
         * @brief Create an address from a sockaddr
         * @param saddr input sockaddr
         */
        Address(const struct sockaddr *saddr);
        /**
         * @brief Copy constructor
         */
        Address(const Address& addr);
        /**
         * @brief Address from ip:port string
         **/
        Address(const std::string& addressFormat);

        /**
         * @brief Assignment constructor
         */
        Address& operator=(const Address& addr);

        /**
         * @brief Get IP address
         * @return ip
         */
        const std::string& getIp() const;
        /**
         * @brief Get port
         * @return port
         */
        unsigned short getPort() const;

        /**
         * @brief Is it empty ?
         * @return the answer
         */
        bool empty() const;

        /**
         * @brief Create a sockaddr structure from the address
         * @param saddr sockaddr structure
         *
         * @note Use a sockaddr_storage stucture to get information for IPv4
         * and IPv6 addresses
         */
        void getSockAddr(struct sockaddr *saddr) const;

        /**
         * @brief Print the address in an output stream
         * @param os output stream
         * @param addr
         */
        friend std::ostream& operator<<(std::ostream& os, const Address& addr);

        /**
         * @brief get a std::string representation of the address
         */
        std::string toString() const;

        /**
         * @brief compare this and addr
         * @param addr
         */
        int compare(const Address& addr) const;

        /**
         * @brief Equality test
         * @param addr1
         * @param addr2
         */
        friend bool operator==(const Address& addr1, const Address& addr2);

        /**
         * @brief Inequality test
         * @param addr1
         * @param addr2
         */
        friend bool operator!=(const Address& addr1, const Address& addr2);

        /**
         * @brief Less test
         * @param addr1
         * @param addr2
         */
        friend bool operator<(const Address& addr1, const Address& addr2);

    private:
        // IP string
        std::string ip;
        // TCP or UDP port
        unsigned short port;
        // 4 or 6
        int ipVersion;

    };
}

#endif /* EPYX_ADDRESS_H */
