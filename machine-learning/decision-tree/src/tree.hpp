// tree.hpp - load a decision tree exported by the Python example
// (export.py, models/tree.txt) and use it to classify rows, C++17.
//
// File format, one node per line after the # comments:
//   id feature threshold left right class
// feature -1 marks a leaf. Rows with x[feature] <= threshold go to left.
#pragma once

#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace dtree {

struct Node {
    int feature;       // -1 at a leaf
    double threshold;  // go left when x[feature] <= threshold
    int left;
    int right;
    int label;  // the predicted class at a leaf
};

namespace detail {

template <class T>
T parse(std::string_view s, std::size_t line, const char* what)
{
    T v{};
    const auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || end != s.data() + s.size())
        throw std::runtime_error("line " + std::to_string(line) + ": bad " + what + ": '" +
                                 std::string(s) + "'");
    return v;
}

inline std::vector<std::string_view> split(std::string_view s, char sep)
{
    std::vector<std::string_view> out;
    std::size_t start = 0;
    while (true) {
        while (sep == ' ' && start < s.size() && s[start] == ' ')
            ++start;
        if (start >= s.size())
            return out;
        const std::size_t end = s.find(sep, start);
        out.push_back(s.substr(start, end - start));
        if (end == std::string_view::npos)
            return out;
        start = end + 1;
    }
}

inline void strip_cr(std::string& s)
{
    if (!s.empty() && s.back() == '\r')
        s.pop_back();
}

}  // namespace detail

class Tree {
public:
    // Read and check a tree file. Every child must come after its parent,
    // which the exporter guarantees and which rules out cycles.
    static Tree load(const std::string& path, std::size_t n_features)
    {
        std::ifstream in(path);
        if (!in)
            throw std::runtime_error("cannot open " + path);
        Tree t;
        std::string text;
        for (std::size_t line = 1; std::getline(in, text); ++line) {
            detail::strip_cr(text);
            if (text.empty() || text[0] == '#')
                continue;
            const auto f = detail::split(text, ' ');
            if (f.size() != 6)
                throw std::runtime_error("line " + std::to_string(line) + ": expected 6 fields");
            const auto id = detail::parse<int>(f[0], line, "node id");
            if (id != static_cast<int>(t.nodes_.size()))
                throw std::runtime_error("line " + std::to_string(line) + ": node " +
                                         std::to_string(id) + " out of order");
            Node n{detail::parse<int>(f[1], line, "feature"),
                   detail::parse<double>(f[2], line, "threshold"),
                   detail::parse<int>(f[3], line, "child"), detail::parse<int>(f[4], line, "child"),
                   detail::parse<int>(f[5], line, "class")};
            if (n.feature >= 0 && (n.feature >= static_cast<int>(n_features) ||
                                   n.left <= id || n.right <= id || !std::isfinite(n.threshold)))
                throw std::runtime_error("line " + std::to_string(line) + ": invalid split");
            if (n.feature < -1)
                throw std::runtime_error("line " + std::to_string(line) + ": bad feature");
            t.nodes_.push_back(n);
        }
        if (t.nodes_.empty())
            throw std::runtime_error(path + ": no nodes");
        for (const Node& n : t.nodes_)
            if (n.feature >= 0 && (n.left >= static_cast<int>(t.nodes_.size()) ||
                                   n.right >= static_cast<int>(t.nodes_.size())))
                throw std::runtime_error(path + ": child index past the last node");
        t.n_features_ = n_features;
        return t;
    }

    // The leaf a row lands in.
    std::size_t leaf(const double* x) const
    {
        std::size_t i = 0;
        while (nodes_[i].feature >= 0) {
            const Node& n = nodes_[i];
            i = static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.left : n.right);
        }
        return i;
    }

    int predict(const double* x) const { return nodes_[leaf(x)].label; }

    // The nodes visited from the root to the leaf.
    std::vector<std::size_t> path(const double* x) const
    {
        std::vector<std::size_t> p{0};
        while (nodes_[p.back()].feature >= 0) {
            const Node& n = nodes_[p.back()];
            p.push_back(static_cast<std::size_t>(x[n.feature] <= n.threshold ? n.left : n.right));
        }
        return p;
    }

    const std::vector<Node>& nodes() const { return nodes_; }
    std::size_t leaves() const
    {
        std::size_t n = 0;
        for (const Node& node : nodes_)
            n += node.feature < 0;
        return n;
    }

private:
    std::vector<Node> nodes_;
    std::size_t n_features_ = 0;
};

// The test rows written by data.py: header, then class and 30 features per line.
struct Rows {
    std::vector<std::string> names;
    std::vector<int> labels;
    std::vector<double> values;  // row r starts at values[r * names.size()]
    std::size_t size() const { return labels.size(); }
    const double* row(std::size_t r) const { return values.data() + r * names.size(); }
};

inline Rows load_rows(const std::string& path)
{
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("cannot open " + path);
    Rows rows;
    std::string text;
    if (!std::getline(in, text))
        throw std::runtime_error(path + ": empty file");
    detail::strip_cr(text);
    const auto header = detail::split(text, ',');
    for (std::size_t i = 1; i < header.size(); ++i)
        rows.names.emplace_back(header[i]);
    for (std::size_t line = 2; std::getline(in, text); ++line) {
        detail::strip_cr(text);
        if (text.empty())
            continue;
        const auto f = detail::split(text, ',');
        if (f.size() != rows.names.size() + 1)
            throw std::runtime_error("line " + std::to_string(line) + ": expected " +
                                     std::to_string(rows.names.size() + 1) + " fields");
        rows.labels.push_back(detail::parse<int>(f[0], line, "class"));
        for (std::size_t i = 1; i < f.size(); ++i) {
            const double v = detail::parse<double>(f[i], line, "number");
            if (!std::isfinite(v))
                throw std::runtime_error("line " + std::to_string(line) + ": not finite");
            rows.values.push_back(v);
        }
    }
    return rows;
}

// splitmix64: the same stream of numbers as data.py's SplitMix64.
class SplitMix64 {
public:
    explicit SplitMix64(std::uint64_t seed) : state_(seed) {}
    std::uint64_t next()
    {
        std::uint64_t z = (state_ += 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }

private:
    std::uint64_t state_;
};

// Which test row to use for each benchmark row: the same pseudo-random order
// as data.py's benchmark_order(), so that no repeating pattern of branches
// flatters the timing.
inline std::vector<std::size_t> benchmark_order(std::size_t n_rows = 1000008,
                                                std::size_t n_test = 114)
{
    SplitMix64 rng(2026);
    std::vector<std::size_t> order(n_rows);
    for (std::size_t& i : order)
        i = static_cast<std::size_t>(rng.next() % n_test);
    return order;
}

}  // namespace dtree
