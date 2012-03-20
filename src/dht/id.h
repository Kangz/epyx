/**
 * DHT Id representing a DHT key and positions in the hash space
 */
#ifndef EPYX_DHT_ID_H
#define EPYX_DHT_ID_H

#define ID_LENGTH 160

namespace Epyx{
namespace DHT {

    class Distance;

    //Represents a DHT Id
    class Id{
        public:
            unsigned char data[ID_LENGTH/8];
            distanceTo(Id* const a, Id* const b, Distance *d)
    }; //TODO uint32_t ?

    //A distance is just an Id but we want different types for readability
    class Distance:Id{};


    //Some operators to work on Id as big numbers
    int operator<(const Id &a, const Id &b);
    int operator==(const Id &a, const Id &b);
}
}

#endif /* EPYX_DHT_ID_H */
