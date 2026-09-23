// array_ops.hpp - a search helper for standard containers.
//
// Insertion, deletion, traversal and size are all provided by the
// standard library, so only the search needs a wrapper: std::find
// returns an iterator, and an index is often more convenient.
#ifndef MYCPLUS_ARRAY_OPS_HPP
#define MYCPLUS_ARRAY_OPS_HPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>

namespace mycplus {

// Return the index of the first element equal to value, or no value.
// std::optional removes the need for a sentinel index such as -1, which
// cannot be represented in an unsigned type anyway.
template <typename Container, typename T>
std::optional<std::size_t> find_index(const Container &c, const T &value)
{
    const auto first = std::begin(c);
    const auto last  = std::end(c);
    const auto it    = std::find(first, last, value);

    if (it == last)
        return std::nullopt;

    return static_cast<std::size_t>(std::distance(first, it));
}

} // namespace mycplus

#endif // MYCPLUS_ARRAY_OPS_HPP
