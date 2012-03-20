#include "id.h"
#include <stdint.h>

namespace Epyx {
namespace DHT {
    Distance* Id::distanceTo(Id* const b, Distance *d) {
        Distance d = new Distance();

        uint8_t *a_data = (uint8_t *) this->data;
        uint8_t *b_data = (uint8_t *) b->data;
        uint8_t *dist = (uint8_t *) d->data;

        for (int i = 0; i < ID_LENGTH; i++) {
            *(dist++) = *(a_data++) ^ *(b_data++);
        }
        return d;
    }


    int operator<(const Id &a, const Id &b){
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < ID_LENGTH; i++) {
            if (*a_data < *b_data) {
                return 1;
            } else if (*a_data > *b_data) {
                return 0;
            }
            a_data++;
            b_data++;
        }
        return 0;
    }


    int operator==(const Id &a, const Id &b){
        uint8_t *a_data = (uint8_t *) a.data;
        uint8_t *b_data = (uint8_t *) b.data;

        for(int i = 0; i < ID_LENGTH; i++){
            if (*a_data != *b_data) {
                return 0;
            }
            a_data++;
            b_data++;
        }
        return 1;
    }
}
}
