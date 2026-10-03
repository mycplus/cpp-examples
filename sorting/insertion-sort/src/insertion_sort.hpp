// insertion_sort.hpp - generic insertion sort for C++17
#ifndef INSERTION_SORT_HPP
#define INSERTION_SORT_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>

// Stable, in place. Needs only bidirectional iterators, so it also sorts
// a std::list. comp(a, b) means "a goes before b", as in std::sort.
template <class BidirIt, class Compare = std::less<>>
void insertion_sort(BidirIt first, BidirIt last, Compare comp = {})
{
    if (first == last)
        return;
    for (BidirIt i = std::next(first); i != last; ++i) {
        auto v = std::move(*i);
        BidirIt j = i;
        for (BidirIt prev = std::prev(j); comp(v, *prev); --prev) {
            *j = std::move(*prev);           // shift the larger element right
            --j;
            if (prev == first)
                break;
        }
        *j = std::move(v);
    }
}

// Binary insertion sort: std::upper_bound finds the insertion point, which
// keeps it stable; std::rotate shifts the block. Random-access iterators.
template <class RandomIt, class Compare = std::less<>>
void binary_insertion_sort(RandomIt first, RandomIt last, Compare comp = {})
{
    for (RandomIt i = first; i != last; ++i)
        std::rotate(std::upper_bound(first, i, *i, comp), i, std::next(i));
}

#endif
