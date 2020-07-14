#ifndef FRAMEWORK_CONTAINER_FIXEDORDEREDMULTIMAP_TPP_
#define FRAMEWORK_CONTAINER_FIXEDORDEREDMULTIMAP_TPP_

template<typename key_t, typename T, typename KEY_COMPARE>
inline FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator::Iterator():
        ArrayList<std::pair<key_t, T>, uint32_t>::Iterator(){}

template<typename key_t, typename T, typename KEY_COMPARE>
inline FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator::Iterator(
        std::pair<key_t, T> *pair):
        ArrayList<std::pair<key_t, T>, uint32_t>::Iterator(pair){}

template<typename key_t, typename T, typename KEY_COMPARE>
inline T FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator::operator*() {
    return ArrayList<std::pair<key_t, T>, uint32_t>::Iterator::value->second;
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline typename FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator
FixedOrderedMultimap<key_t, T, KEY_COMPARE>::begin() const {
    return Iterator(&theMap[0]);
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline typename FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator
FixedOrderedMultimap<key_t, T, KEY_COMPARE>::end() const {
    return Iterator(&theMap[_size]);
}


template<typename key_t, typename T, typename KEY_COMPARE>
inline size_t FixedOrderedMultimap<key_t, T, KEY_COMPARE>::size() const {
    return _size;
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline T* FixedOrderedMultimap<key_t, T, KEY_COMPARE>::Iterator::operator->() {
    return &ArrayList<std::pair<key_t, T>, uint32_t>::Iterator::value->second;
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline FixedOrderedMultimap<key_t, T, KEY_COMPARE>::FixedOrderedMultimap(
        size_t maxSize): theMap(maxSize), _size(0) {}


template<typename key_t, typename T, typename KEY_COMPARE>
inline ReturnValue_t FixedOrderedMultimap<key_t, T, KEY_COMPARE>::insert(
        key_t key, T value, Iterator *storedValue) {
    if (_size == theMap.maxSize()) {
        return MAP_FULL;
    }
    uint32_t position = findNicePlace(key);
    // Compiler might emitt warning because std::pair is not a POD type (yet..)
    // See: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2342.htm#std::pair-example
    // Should still work without issues.
    std::memmove(&theMap[position + 1], &theMap[position],
            (_size - position) * sizeof(std::pair<key_t,T>));
    theMap[position].first = key;
    theMap[position].second = value;
    ++_size;
    if (storedValue != nullptr) {
        *storedValue = Iterator(&theMap[position]);
    }
    return HasReturnvaluesIF::RETURN_OK;
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline ReturnValue_t FixedOrderedMultimap<key_t, T, KEY_COMPARE>::insert(
        std::pair<key_t, T> pair) {
    return insert(pair.fist, pair.second);
}

template<typename key_t, typename T, typename KEY_COMPARE>
inline ReturnValue_t FixedOrderedMultimap<key_t, T, KEY_COMPARE>::exists(
        key_t key) const {
    ReturnValue_t result = KEY_DOES_NOT_EXIST;
    if (findFirstIndex(key) < _size) {
        result = HasReturnvaluesIF::RETURN_OK;
    }
    return result;
}

#endif /* FRAMEWORK_CONTAINER_FIXEDORDEREDMULTIMAP_TPP_ */
