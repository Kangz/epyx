/**
 * @file uri.h
 * @brief Uniform Resource Identifier
 */

#ifndef EPYX_URI_H
#define EPYX_URI_H

#include <string>
#include "address.h"

namespace Epyx
{
    /**
     * @class URI
     *
     * @brief URI Storage
     *
     * An URI is a string in the form of "http://192.168.42.1:1337/path/to/file"
     * This is internally splitted into
     * - a scheme (http)
     * - an Address (IP:port)
     * - an absolute path (/path/to/file)
     */
    class URI
    {
    public:
        /**
         * @brief Empty constructor
         */
        URI();
        /**
         * @brief Full constructor
         * @param scheme
         * @param addr
         * @param path
         */
        URI(const std::string& scheme, const Address& addr, const std::string& path);
        /**
         * @brief Constructor with parsing
         * @param uri
         */
        URI(const std::string& uri);
        /**
         * @brief Copy constructor
         * @param uri
         */
        URI(const URI& uri);
        /**
         * @brief Assignment constructor
         * @param uri
         */
        URI& operator=(const URI& uri);

        /**
         * @brief Get scheme
         * @return this->scheme
         */
        const std::string& getScheme() const;
        /**
         * @brief Get address
         * @return this->addr
         */
        const Address& getAddress() const;
        /**
         * @brief Get absolute path
         * @return this->path
         */
        const std::string& getPath() const;

        /**
         * @brief Print the address in an output stream
         * @param os output stream
         * @param uri
         */
        friend std::ostream& operator<<(std::ostream& os, const URI& uri);

        /**
         * @brief get a std::string representation of the URI
         */
        std::string toString() const;

    private:
        std::string scheme;
        Address addr;
        std::string path;
    };
}


#endif /* EPYX_URI_H */

