// heap_sort.hpp - generic heap sort for C++17
#ifndef HEAP_SORT_HPP
#define HEAP_SORT_HPP

#include <functional>
#include <iterator>
#include <utility>

namespace detail {

template <class RandomIt, class Diff, class Compare>
void sift_down(RandomIt first, Diff root, Diff n, Compare& comp)
{
    for (;;) {
        Diff child = 2 * root + 1;
        if (child >= n)
            return;
        if (child + 1 < n && comp(first[child], first[child + 1]))
            ++child;                                  // the larger child
        if (!comp(first[root], first[child]))
            return;
        std::iter_swap(first + root, first + child);
        root = child;
    }
}

}  // namespace detail

// In place, O(n log n) worst case, not stable. comp(a, b) means "a goes
// before b"; the heap is a max-heap with respect to comp.
template <class RandomIt, class Compare = std::less<>>
void heap_sort(RandomIt first, RandomIt last, Compare comp = {})
{
    using Diff = typename std::iterator_traits<RandomIt>::difference_type;
    Diff n = last - first;
    for (Diff i = n / 2; i-- > 0; )                   // build the heap
        detail::sift_down(first, i, n, comp);
    for (Diff end = n; end > 1; --end) {              // extract the largest
        std::iter_swap(first, first + (end - 1));
        detail::sift_down(first, Diff{0}, end - 1, comp);
    }
}

#endif
