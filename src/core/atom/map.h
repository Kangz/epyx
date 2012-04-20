/**
 * @file map.h
 * @brief Thread-safe map
 */

#ifndef EPYX_ATOM_MAP_H
#define EPYX_ATOM_MAP_H

#include "../mutex.h"
#include <map>

namespace Epyx
{
    namespace atom
    {
        /**
         * @class Map
         * @brief Thread-safe STL map
         */
        template <typename TKey, typename TVal> class Map
        {
        public:
            /**
             * @brief iterator
             */
            typedef typename std::map<TKey, TVal>::iterator iterator;
            typedef typename std::map<TKey, TVal>::const_iterator const_iterator;

            /**
             * @brief Insert an element
             * @param key
             * @param value
             */
            void set(TKey key, TVal value);

            /**
             * @brief Get an element
             * @param key
             * @param defval Default value
             * @return map[key] if it exists, defval otherwise
             */
            TVal get(TKey key, TVal defval);

            /**
             * @brief Erase an element
             * @param key
             */
            void unset(TKey key);

            /**
             * @brief Get and unset
             * @param key
             * @param defval default return value
             * @return unsetted value
             *
             * This may be useful to call a destrcuor on a registered value
             */
            TVal getUnset(TKey key, TVal defval);

            /**
             * @brief Is it empty ?
             * @return true if it is empty
             */
            bool empty();

            /**
             * @brief Lock map and begin iteration
             * @return iterator
             */
            iterator beginLock();

            /**
             * @brief Tell wether iterator is ended
             * @param it
             * @return true on and
             */
            bool isEnd(const iterator& it);

            /**
             * @brief End iteration and unlock map
             */
            void endUnlock();

        private:
            std::map<TKey, TVal> map;
            Mutex mut;
        };
    }
}

#include "map-detail.h"

#endif /* EPYX_ATOM_MAP_H */

