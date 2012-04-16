/**
 * @file tls-pointer.h
 * @brief TLS pointer definition.
 */
#ifndef EPYX_TLS_POINTER_H
#define EPYX_TLS_POINTER_H

#include <pthread.h>
#include "assert.h"
#include "exception.h"


namespace Epyx
{
    /**
     * @class TLSPointer
     *
     * @brief A pointer with a per-thread value
     *
     * Thread Local Storage is used when you want a variable that is local to
     * a thread : it acts as a global variable that is different in each thread.
     * The optional constructor argument is a function that will be used to
     * populate the pointer when new threads are spawned. The destructor is
     * called only whe the thread is terminated from the outside : when you
     * replace the pointer or when you end the thread normally, the estructor
     * is not called.
     *
     * @tparam T the base type of the pointers contained in the TLSPointer
     */
    template<typename T> class TLSPointer
    {
    public:
        /**
         * @brief A TLSPointer constructor
         * @param destructor (optional) a pointer to a destructor function
         */
        TLSPointer(void (*destructor)(T*) = &_default_destructor);

        /**
         * @brief A TLSPointer constructor
         * @param constructor a pointer to a constructor function
         * @param destructor (optional) a pointer to a destructor function
         */
        TLSPointer(T* (*constructor)(), void (*destructor)(T*) = &_default_destructor);

        /**
         * @brief the TLSPointer destructor
         */
        ~TLSPointer();

        /**
         * @brief get the per thread value of the pointer
         * @return the pointer value
         */
        T* get();

        /**
         * @brief sets the value of the pointer
         * @param new_value (optional) the new value of the pointer, null if omitted
         */
        void reset(T* new_value = NULL);

        /**
         * @brief equivalent to {temp = tls.get(); tls.reset(); return temp}
         * @return the value of the pointer
         */
        T* release();

    private:
        // Disable copy construction and assignment.
        TLSPointer(const TLSPointer&);
        const TLSPointer& operator=(const TLSPointer&);

        void (*destructor)(T*);
        T* (*constructor)();
        pthread_key_t key;

        // Destructor type with void*
        typedef void(void_destructor_func) (void*);

        static void _default_destructor(T* object);
    };

}

#include "tls-pointer-detail.h"

#endif /* EPYX_TLS_POINTER_H */
