#ifndef AD_CORE_SORT_H_
#define AD_CORE_SORT_H_

#include <iterator>
#include <memory>
#include <algorithm>
#include <utility>

#include "ad_types.h"
#include "ad_util.h"

namespace ad
{

template <class BidirIt, class Compare>
Void insertionSort(BidirIt first, BidirIt last, Compare comp)
{
    if (first == last) {
        return;
    }
    
    auto i = first, j = first, k = first;
    typename std::iterator_traits<BidirIt>::value_type tmp;

    for (++i; i != last; ++i) {
        if (comp(*i, *first)) {
            j = i;
            tmp = std::move(*i);
            std::move_backward(first, i, ++j);
            *first = std::move(tmp);
        } else {
            j = k = i;
            --j;
            tmp = std::move(*i);
            while (comp(tmp, *j)) {
                *k = std::move(*j);
                --j;
                --k;
            }
            *k = std::move(tmp);
        }
    }
}

template <class BidirIt>
Void insertionSort(BidirIt first, BidirIt last)
{
    insertionSort(first, last, std::less<typename
        std::iterator_traits<BidirIt>::value_type>());
}

template <class RandomIt, class Compare>
Void mergeSort(RandomIt first, RandomIt last, Compare comp)
{
    auto size = last - first;
    using ValueType = typename std::iterator_traits<
        RandomIt>::value_type;

    if (size < 2) {
        return;
    }

    const decltype(size) THRESHOLD = 32;

    for (decltype(size) i = 0; i < size; i += THRESHOLD) {
        insertionSort(first + i, first + std::min(i + THRESHOLD,
            size), comp);
    }

    if (size <= THRESHOLD) {
        return;
    }

    auto ret = std::get_temporary_buffer<ValueType>(size);
    using BuffPtr = decltype(ret.first);
    if (ret.second < size) {
        throw std::bad_alloc();
    }
    auto buff = ret.first;
    decltype(size) num = 0;
    for (decltype(size) i = THRESHOLD; i < size; i <<= 1, ++num) {
        if (num & 1) {
            decltype(size) j = 0;
            for (; j < size - i; j += (i << 1)) {
                std::merge(
                    std::move_iterator<BuffPtr>(buff + j),
                    std::move_iterator<BuffPtr>(buff + j + i),
                    std::move_iterator<BuffPtr>(buff + j + i),
                    std::move_iterator<BuffPtr>(buff +
                        std::min(size, j + (i << 1))),
                    first + j,
                    comp
                );
            }
            if (j < size) {
                std::move(buff + j, buff + size, first + j);
            }
        } else {
            decltype(size) j = 0;
            for (; j < size - i; j += (i << 1)) {
                std::merge(
                    std::move_iterator<RandomIt>(first + j),
                    std::move_iterator<RandomIt>(first + j + i),
                    std::move_iterator<RandomIt>(first + j + i),
                    std::move_iterator<RandomIt>(first +
                        std::min(size, j + (i << 1))),
                    buff + j,
                    comp
                );
            }
            if (j < size) {
                std::move(first + j, first + size, buff + j);
            }
        }
    }

    if (num & 1) {
        std::move(buff, buff + size, first);
    }

    std::return_temporary_buffer(buff);
}

template <class RandomIt>
Void mergeSort(RandomIt first, RandomIt last)
{
    return mergeSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

}

#endif