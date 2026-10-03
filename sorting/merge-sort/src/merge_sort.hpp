// merge_sort.hpp - generic top-down merge sort for C++17
#ifndef MERGE_SORT_HPP
#define MERGE_SORT_HPP

#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace detail {

template <class RandomIt, class T, class Compare>
void merge_sort_rec(RandomIt first, RandomIt last, std::vector<T>& buf, Compare& comp)
{
    auto n = last - first;
    if (n < 2)
        return;
    RandomIt mid = first + n / 2;
    merge_sort_rec(first, mid, buf, comp);
    merge_sort_rec(mid, last, buf, comp);
    if (!comp(*mid, *std::prev(mid)))            // halves already in order
        return;

    buf.clear();                                 // capacity is kept: no reallocation
    RandomIt i = first, j = mid;
    while (i != mid && j != last)
        buf.push_back(comp(*j, *i) ? std::move(*j++) : std::move(*i++));  // ties: left first
    while (i != mid)
        buf.push_back(std::move(*i++));
    std::move(buf.begin(), buf.end(), first);    // the rest of [j, last) is already in place
}

}  // namespace detail

// Stable, O(n log n) comparisons on every input, O(n) extra memory.
// comp(a, b) means "a goes before b", as in std::sort.
template <class RandomIt, class Compare = std::less<>>
void merge_sort(RandomIt first, RandomIt last, Compare comp = {})
{
    using T = typename std::iterator_traits<RandomIt>::value_type;
    std::vector<T> buf;
    buf.reserve(static_cast<std::size_t>(last - first));   // one allocation
    detail::merge_sort_rec(first, last, buf, comp);
}

#endif
