#include "id.h"
#include <stdint.h>

namespace Epyx {
namespace DHT {

    std::ostream& operator<<(std::ostream& os, const Id& self) {
        const char hexa[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        uint8_t *dist = (uint8_t *) self.data;
        int loop = 0;

        for (int i = 0; i < ID_STORAGE_SIZE; i++) {
            if(loop%2 == 0 && loop != 0){
                os << ':';
            }
            os << hexa[*(dist)/16] << hexa[*(dist)%16];
            loop ++;
            dist ++;
        }
        return os;
    }

    Distance::Distance(Id* const a, Id* const b) {
        uint8_t *a_data = (uint8_t *) a->data;
        uint8_t *b_data = (uint8_t *) b->data;
        uint8_t *dist = (uint8_t *) this->data;

        firstActive = 0;
        uint8_t* firstActiveChar = NULL;

        for (int i = 0; i < ID_STORAGE_SIZE; i++) {
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

        unsigned char active = *firstActiveChar;

        for(int i=0; i<8; i++){
            if( (0x80 & active) == 0 ){
                firstActive ++;
                active <<= 1;
            }
        }
    }

    int Distance::firstActiveBit() const{
        return firstActive;
    }

    std::ostream& operator<<(std::ostream& os, const Distance& d){
        Id* const i = (Id*) &d;
        os << *i << '%' << d.firstActiveBit();
        return os;
    };

    bool operator<(const Id &a, const Id &b){
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < ID_STORAGE_SIZE; i++) {
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


    bool operator==(const Id &a, const Id &b){
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < ID_STORAGE_SIZE; i++){
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
