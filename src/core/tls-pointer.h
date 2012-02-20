/**
 * Thread Local Storage astraction layer
 */
#ifndef EPYX_TLS_POINTER_H
#define EPYX_TLS_POINTER_H

#include <pthread.h>
#include "assert.h"
#include "exception.h"


namespace Epyx {

    template<typename T> class TLSPointer {
    public:
        static void _default_destructor(T* object);

        TLSPointer(void (*destructor)(T*) = &_default_destructor);
        TLSPointer(T* (*constructor)(), void (*destructor)(T*) = &_default_destructor);
        ~TLSPointer();

        T* get();
        void reset(T* new_value=NULL);
        T* release();

    private:
        // Disable copy construction and assignment.
        TLSPointer(const TLSPointer&);
        const TLSPointer& operator=(const TLSPointer&);

        void (*destructor)(T*);
        T* (*constructor)();
        pthread_key_t* key;

        typedef void(void_destructor_func)(void*);
    };

}

#include "tls-pointer-detail.h"

#endif /* EPYX_TLS_POINTER_H */
