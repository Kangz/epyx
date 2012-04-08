/**
 * DHT Id representing a DHT key and positions in the hash space
 */
#ifndef EPYX_DHT_ID_H
#define EPYX_DHT_ID_H

#include <iostream>

#define ID_LENGTH 160
#define ID_STORAGE_SIZE (ID_LENGTH/8)

namespace Epyx
{
namespace DHT
{

    class Distance;

    //Represents a DHT Id
    class Id
    {
        public:
            unsigned char data[ID_STORAGE_SIZE];

    }; //TODO uint32_t ?
    std::ostream& operator<<(std::ostream& os, const Id& id);
    std::istream& operator>>(std::istream& in, Id& id);
    //A distance is just an Id but we want different types to add methods
    class Distance: public Id
    {
        public:
            Distance(const Id* a, const Id* b);
            int firstActiveBit() const;

        private:
            int firstActive;
    };
    std::ostream& operator<<(std::ostream& os, const Distance& d);


    //Some operators to work on Id as big numbers
    bool operator<(const Id &a, const Id &b);
    bool operator==(const Id &a, const Id &b);
}
}

#endif /* EPYX_DHT_ID_H */
