/**
 * @file dht/id.cpp
 * @brief Implements the methods of Ids and Distances
 */

#include "id.h"
#include "../core/common.h"

namespace Epyx
{
namespace DHT
{

    Id::Id() {
        this->reset();
    }

    Id::Id(Initialisation init) {
        //Use the right initialisation method
        switch (init) {
        case INIT_ZERO:
            this->reset();
            return;
        case INIT_RANDOM:
            this->randomize();
            return;
        }
        throw FailException("DHT::Id", "Invalid initialisation method");
    }

    Id::Id(const Id& id) {
        for (int i = 0; i < Id::STORAGE_SIZE; i++) {
            data[i] = id.data[i];
        }
    }

    Id& Id::operator=(const Id& id) {
        if (this != &id) {
            for (int i = 0; i < Id::STORAGE_SIZE; i++) {
                this->data[i] = id.data[i];
            }
        }
        return *this;
    }

    void Id::randomize() {
        for (int i = 0; i < DHT::Id::STORAGE_SIZE; i++) {
            data[i] = rand()%256;
        }
    }

    void Id::reset() {
        for (int i = 0; i < DHT::Id::STORAGE_SIZE; i++) {
            data[i] = 0;
        }
    }

    std::ostream& operator<<(std::ostream& os, const Id& self) {
        const char hexa[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        uint8_t *dist = (uint8_t *) self.data;
        int loop = 0;

        //Handle hexa digits two at a time and inset colons
        for (int i = 0; i < Id::STORAGE_SIZE; i++) {
            if(loop%2 == 0 && loop != 0){
                os << ':';
            }
            os << hexa[*(dist)/16] << hexa[*(dist)%16];
            loop ++;
            dist ++;
        }
        return os;
    }

    //TODO pop an error if it needs to
    std::istream& operator>>(std::istream& in, Id& id) {
        uint8_t* pointer = (uint8_t*) id.data;
        uint8_t n1, n2;

        for(int i = 0; i<Id::STORAGE_SIZE; i++){
            n1 = in.get();
            n2 = in.get();

            uint8_t value = 0;

            //Compute the char value for two digits
            n1 = n1>'9' ? (n1-'a'+10) : (n1-'0');
            n2 = n2>'9' ? (n2-'a'+10) : (n2-'0');
            value = 16*n1 + n2;

            *pointer = value;
            pointer ++;

            //delete colons
            if(i != Id::STORAGE_SIZE-1 && i%2 ==1){
                 in.get();
            }
        }
        return in;
    }

    void idForString(Id& id, const std::string& toCipher) {
        const unsigned char* ciphered = reinterpret_cast<const unsigned char*>(crypto::SHA256(toCipher).c_str());
        for(int i = 0; i<Id::STORAGE_SIZE; i++) {
            id.data[i] = ciphered[i];
        }
    }


    Distance::Distance(const Id* a, const Id* b) {
        initSelf(a, b);
    }

    Distance::Distance(const Id& a, const Id& b) {
        initSelf(&a, &b);
    }

    int Distance::firstActiveBit() const {
        return firstActive;
    }

    void Distance::initSelf(const Id* a, const Id* b) {
        uint8_t *a_data = (uint8_t *) a->data;
        uint8_t *b_data = (uint8_t *) b->data;
        uint8_t *dist = (uint8_t *) this->data;

        firstActive = 0;
        uint8_t* firstActiveChar = NULL;

        //Finds blocks of 0
        for (int i = 0; i < Id::STORAGE_SIZE; i++) {
            *(dist) = *(a_data++) ^ *(b_data++);
            if (firstActiveChar == NULL ) {
                if (*(dist) == 0 ) {
                    firstActive += 8;
                }else {
                    firstActiveChar = dist;
                }
            }
            dist ++;
        }

        dist --;

        //If it is still NULL that means dist === 0
        if (firstActiveChar == NULL) {
            firstActive = Id::LENGTH - 1;
            return;
        }

        unsigned char active = *firstActiveChar;

        //Search for the first 1 inside the block (there must be some asm for this)
        for(int i=0; i<8; i++){
            if( (0x80 & active) == 0 ){
                firstActive ++;
                active <<= 1;
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const Distance& d) {
        Id* const i = (Id*) &d;
        os << *i << '%' << d.firstActiveBit();
        return os;
    };

    bool operator<(const Id &a, const Id &b) {
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < Id::STORAGE_SIZE; i++) {
            if (*a_data < *b_data) {
                return true;
            } else if (*a_data > *b_data) {
                return false;
            }
            a_data++;
            b_data++;
        }
        return false;
    }


    bool operator==(const Id &a, const Id &b) {
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < Id::STORAGE_SIZE; i++){
            if (*a_data != *b_data) {
                return false;
            }
            a_data++;
            b_data++;
        }
        return true;
    }
}
}
