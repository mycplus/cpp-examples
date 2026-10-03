// selection_sort.hpp - generic selection sort for C++17
#ifndef SELECTION_SORT_HPP
#define SELECTION_SORT_HPP

#include <algorithm>
#include <functional>
#include <iterator>

// In place, at most n - 1 swaps. Not stable. Needs only forward iterators,
// so it also sorts a std::forward_list. comp(a, b) means "a goes before b".
template <class ForwardIt, class Compare = std::less<>>
void selection_sort(ForwardIt first, ForwardIt last, Compare comp = {})
{
    for (ForwardIt i = first; i != last; ++i) {
        ForwardIt min = std::min_element(i, last, comp);  // first of the smallest
        if (min != i)
            std::iter_swap(i, min);
    }
}

#endif
