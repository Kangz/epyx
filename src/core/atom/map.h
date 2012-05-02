/**
 * @file map.h
 * @brief Thread-safe map
 */

#ifndef EPYX_ATOM_MAP_H
#define EPYX_ATOM_MAP_H

#include "../mutex.h"
#include <map>

#ifndef EPYX_ATOM_MAP_DEBUG
#define EPYX_ATOM_MAP_DEBUG 0
#endif

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
            /**
             * @brief constant iterator
             */
            typedef typename std::map<TKey, TVal>::const_iterator const_iterator;

            /**
             * @brief Constructor
             */
            Map();

            /**
             * @brief Insert an element
             * @param key
             * @param value
             */
            void set(TKey key, TVal value);

            /**
             * @brief Get an element and lock the map to do something with
             * @param key
             * @param defval Default value
             * @return map[key] if it exists, defval otherwise
             *
             * @note Call endUnlock() to unlock the map
             */
            TVal getAndLock(TKey key, TVal defval);

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
             * @return true if there was an element which is now deleted
             */
            bool unset(TKey key);

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
             * @brief Clear the map
             */
            void clear();

            /**
             * @brief Clear the map and disallow any change
             */
            void clearForEver();

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
            bool isEnd(const const_iterator& it);

            /**
             * @brief End iteration and unlock map
             */
            void endUnlock();

        private:
            bool readOnly;
            std::map<TKey, TVal> map;
            Mutex mut;
        };
    }
}

#include "map-detail.h"

#endif /* EPYX_ATOM_MAP_H */

