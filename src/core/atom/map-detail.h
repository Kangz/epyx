/**
 * @file map-detail.h
 * @brief Template part of atom::map
 */

#ifndef EPYX_ATOM_MAP_DETAIL_H
#define EPYX_ATOM_MAP_DETAIL_H

#include "../assert.h"
#include "../log.h"

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
        void Map<TKey, TVal>::setLocked(TKey key, TVal value) {
            if (!readOnly)
                map[key] = value;
        }

        template <typename TKey, typename TVal>
        TVal Map<TKey, TVal>::getAndLock(TKey key, TVal defval) {
            mut.lock();
#if EPYX_ATOM_MAP_DEBUG
            log::debug << "Mutex " << this << " !lock" << log::endl;
#endif
            const_iterator it = map.find(key);
            return ((it == map.end()) ? defval : it->second);
            // No mut.unlock();
        }

        template <typename TKey, typename TVal>
        TVal Map<TKey, TVal>::get(TKey key, TVal defval) {
            TVal retval = this->getAndLock(key, defval);
#if EPYX_ATOM_MAP_DEBUG
            log::debug << "Mutex " << this << " !unlock (get)" << log::endl;
#endif
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
        void Map<TKey, TVal>::clear(bool isLocked) {
            if (!isLocked)
                mut.lock();
            if (!readOnly)
                map.clear();
            if (!isLocked)
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
        long unsigned int Map<TKey, TVal>::size() {
            mut.lock();
            long unsigned int result = map.size();
            mut.unlock();
            return result;
        }

        template <typename TKey, typename TVal>
        typename Map<TKey, TVal>::iterator Map<TKey, TVal>::beginLock() {
            mut.lock();
#if EPYX_ATOM_MAP_DEBUG
            log::debug << "Mutex " << this << " !lock (iter)" << log::endl;
#endif
            return map.begin();
        }

        template <typename TKey, typename TVal>
        bool Map<TKey, TVal>::isEnd(const Map<TKey, TVal>::const_iterator& it) {
            return (it == map.end());
        }

        template <typename TKey, typename TVal>
        void Map<TKey, TVal>::endUnlock() {
#if EPYX_ATOM_MAP_DEBUG
            log::debug << "Mutex " << this << " !unlock" << log::endl;
#endif
            mut.unlock();
        }

    }
}

#endif /* EPYX_ATOM_MAP_DETAIL_H */

