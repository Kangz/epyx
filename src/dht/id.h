/**
 * @file dht/id.h
 * @brief DHT Id representing a DHT key and positions in the hash space
 */
#ifndef EPYX_DHT_ID_H
#define EPYX_DHT_ID_H

#include <iostream>

namespace Epyx
{
namespace DHT
{

    class Distance;

    /**
     * @class Id
     * @brief represents positions in the DHT hash space
     */
    class Id
    {
    public:
        static const int LENGTH = 160;
        static const int STORAGE_SIZE = 160/8;
        unsigned char data[STORAGE_SIZE];

    }; //TODO uint32_t ?


    std::ostream& operator<<(std::ostream& os, const Id& id);
    std::istream& operator>>(std::istream& in, Id& id);

    /**
     * @class Distance
     * @brief a distance is just an Id but we want different types to add methods
     */
    class Distance: public Id
    {
    public:
        /**
         * @brief the constructor of a Distance from 2 Ids
         * @param a one of the Ids
         * @param b the other
         */
        Distance(const Id* a, const Id* b);
        /**
         * @brief the constructor of a Distance from 2 Ids
         * @param a one of the Ids
         * @param b the other
         */
        Distance(const Id& a, const Id& b);

        /**
         * @brief returns the index first non null bit
         * @return log2(distance) kind of
         */
        int firstActiveBit() const;

    private:
        int firstActive;

        void initSelf(const Id* a, const Id* b);
    };

    std::ostream& operator<<(std::ostream& os, const Distance& d);
    bool operator<(const Id &a, const Id &b);
    bool operator==(const Id &a, const Id &b);
}
}

#endif /* EPYX_DHT_ID_H */
