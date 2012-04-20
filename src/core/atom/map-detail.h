/**
 * @file map-detail.h
 * @brief Template part of atom::map
 */

#ifndef EPYX_ATOM_MAP_DETAIL_H
#define EPYX_ATOM_MAP_DETAIL_H

namespace Epyx
{
    namespace atom
    {

        template <typename TKey, typename TVal>
        Map<TKey, TVal>::Map()
        : readOnly(false) {
        }

        template <typename TKey, typename TVal>
        void Map<TKey, TVal>::set(TKey key, TVal value) {
            mut.lock();
            if (!readOnly)
                map[key] = value;
            mut.unlock();
        }

        template <typename TKey, typename TVal>
        TVal Map<TKey, TVal>::get(TKey key, TVal defval) {
            TVal retval;
            mut.lock();
            const_iterator it = map.find(key);
            retval = ((it == map.end()) ? defval : it->second);
            mut.unlock();
            return retval;
        }

        template <typename TKey, typename TVal>
        bool Map<TKey, TVal>::unset(TKey key) {
            bool result;
            mut.lock();
            if (!readOnly) {
                result = (map.erase(key) > 0);
            }
            mut.unlock();
            return result;
        }

        template <typename TKey, typename TVal>
        TVal Map<TKey, TVal>::getUnset(TKey key, TVal defval) {
            TVal retval;
            mut.lock();
            iterator it = map.find(key);
            if (it == map.end()) {
                retval = defval;
            } else {
                retval = it->second;
                if (!readOnly)
                    map.erase(it);
            }
            mut.unlock();
            return retval;
        }

        template <typename TKey, typename TVal>
        void Map<TKey, TVal>::clear() {
            mut.lock();
            if (!readOnly)
                map.clear();
            mut.unlock();
        }

        template <typename TKey, typename TVal>
        void Map<TKey, TVal>::clearForEver() {
            mut.lock();
            readOnly = true;
            map.clear();
            mut.unlock();
        }

        template <typename TKey, typename TVal>
        bool Map<TKey, TVal>::empty() {
            mut.lock();
            bool result = map.empty();
            mut.unlock();
            return result;
        }

        template <typename TKey, typename TVal>
        typename Map<TKey, TVal>::iterator Map<TKey, TVal>::beginLock() {
            mut.lock();
            return map.begin();
        }

        template <typename TKey, typename TVal>
        bool Map<TKey, TVal>::isEnd(const Map<TKey, TVal>::const_iterator& it) {
            return (it == map.end());
        }

        template <typename TKey, typename TVal>
        void Map<TKey, TVal>::endUnlock() {
            mut.unlock();
        }

    }
}

#endif /* EPYX_ATOM_MAP_DETAIL_H */

