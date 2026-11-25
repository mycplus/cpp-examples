// knn.hpp - k-nearest neighbors classification, brute force, C++17.
//
// The same rules as the Python version (knn.py), so both make the same
// predictions on the same data:
//   neighbors with equal distances are ordered by training-row index;
//   a tied vote goes to the smallest class label.
#pragma once

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace knn {

// Rows stored one after another: element (r, c) is values[r * cols + c].
struct Dataset {
    std::size_t rows = 0;
    std::size_t cols = 0;
    std::vector<double> values;
    std::vector<int> labels;

    const double* row(std::size_t r) const { return values.data() + r * cols; }
};

enum class Metric { euclidean, manhattan };

inline double distance(const double* a, const double* b, std::size_t n, Metric metric)
{
    double sum = 0.0;
    if (metric == Metric::euclidean) {
        for (std::size_t i = 0; i < n; ++i) {
            const double d = a[i] - b[i];
            sum += d * d;
        }
        return std::sqrt(sum);
    }
    for (std::size_t i = 0; i < n; ++i)
        sum += std::fabs(a[i] - b[i]);
    return sum;
}

struct Neighbor {
    double distance;
    std::size_t index;  // training row
};

// The k training rows nearest to query, nearest first.
inline std::vector<Neighbor> nearest(const Dataset& train, const double* query, std::size_t k,
                                     Metric metric = Metric::euclidean)
{
    if (k == 0 || k > train.rows)
        throw std::invalid_argument("k must be between 1 and the number of training rows");
    std::vector<Neighbor> all(train.rows);
    for (std::size_t r = 0; r < train.rows; ++r)
        all[r] = {distance(query, train.row(r), train.cols, metric), r};
    const auto closer = [](const Neighbor& a, const Neighbor& b) {
        return a.distance < b.distance || (a.distance == b.distance && a.index < b.index);
    };
    // nth_element puts the k nearest first in O(n); only those k are sorted.
    const auto kth = all.begin() + static_cast<std::ptrdiff_t>(k);
    std::nth_element(all.begin(), kth - 1, all.end(), closer);
    std::sort(all.begin(), kth, closer);
    all.resize(k);
    return all;
}

// Majority vote of the k nearest; a tie goes to the smallest label.
inline int predict_one(const Dataset& train, const double* query, std::size_t k,
                       Metric metric = Metric::euclidean)
{
    const std::vector<Neighbor> nbrs = nearest(train, query, k, metric);
    std::vector<std::pair<int, int>> votes;  // (label, count), sorted by label
    for (const Neighbor& n : nbrs) {
        const int label = train.labels[n.index];
        auto it = std::lower_bound(votes.begin(), votes.end(), std::make_pair(label, 0),
                                   [](const auto& a, const auto& b) { return a.first < b.first; });
        if (it == votes.end() || it->first != label)
            it = votes.insert(it, {label, 0});
        ++it->second;
    }
    std::pair<int, int> best = votes.front();
    for (const auto& v : votes)
        if (v.second > best.second)  // strictly more: the smaller label keeps a tie
            best = v;
    return best.first;
}

inline std::vector<int> predict(const Dataset& train, const Dataset& queries, std::size_t k,
                                Metric metric = Metric::euclidean)
{
    if (queries.cols != train.cols)
        throw std::invalid_argument("queries and training rows have different columns");
    std::vector<int> out(queries.rows);
    for (std::size_t r = 0; r < queries.rows; ++r)
        out[r] = predict_one(train, queries.row(r), k, metric);
    return out;
}

// Scale every column to mean 0 and standard deviation 1 using the training
// rows only (population standard deviation, as scikit-learn's StandardScaler).
inline void standardize(Dataset& train, Dataset& test)
{
    for (std::size_t c = 0; c < train.cols; ++c) {
        double mean = 0.0;
        for (std::size_t r = 0; r < train.rows; ++r)
            mean += train.values[r * train.cols + c];
        mean /= static_cast<double>(train.rows);
        double var = 0.0;
        for (std::size_t r = 0; r < train.rows; ++r) {
            const double d = train.values[r * train.cols + c] - mean;
            var += d * d;
        }
        const double sd = std::sqrt(var / static_cast<double>(train.rows));
        if (sd == 0.0)
            throw std::invalid_argument("a column is constant on the training rows");
        for (Dataset* d : {&train, &test})
            for (std::size_t r = 0; r < d->rows; ++r)
                d->values[r * d->cols + c] = (d->values[r * d->cols + c] - mean) / sd;
    }
}

namespace detail {

inline double parse_double(std::string_view s, std::size_t line)
{
    double v = 0.0;
    const auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || end != s.data() + s.size() || !std::isfinite(v))
        throw std::runtime_error("line " + std::to_string(line) + ": not a number: '" +
                                 std::string(s) + "'");
    return v;
}

inline std::vector<std::string_view> split(std::string_view s)
{
    std::vector<std::string_view> fields;
    for (std::size_t start = 0;;) {
        const std::size_t comma = s.find(',', start);
        fields.push_back(s.substr(start, comma - start));
        if (comma == std::string_view::npos)
            return fields;
        start = comma + 1;
    }
}

}  // namespace detail

// Read the CSV written by data.py: header, then "split,class,feature,...".
inline std::pair<Dataset, Dataset> load_split_csv(const std::string& path)
{
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("cannot open " + path);
    std::string text;
    std::getline(in, text);
    if (!text.empty() && text.back() == '\r')
        text.pop_back();
    const std::size_t cols = detail::split(text).size() - 2;
    Dataset train, test;
    train.cols = test.cols = cols;
    for (std::size_t line = 2; std::getline(in, text); ++line) {
        if (!text.empty() && text.back() == '\r')
            text.pop_back();
        if (text.empty())
            continue;
        const std::vector<std::string_view> f = detail::split(text);
        if (f.size() != cols + 2)
            throw std::runtime_error("line " + std::to_string(line) + ": expected " +
                                     std::to_string(cols + 2) + " fields");
        Dataset* d = f[0] == "train" ? &train : f[0] == "test" ? &test : nullptr;
        if (d == nullptr)
            throw std::runtime_error("line " + std::to_string(line) + ": split must be train or test");
        d->labels.push_back(static_cast<int>(detail::parse_double(f[1], line)));
        for (std::size_t c = 0; c < cols; ++c)
            d->values.push_back(detail::parse_double(f[c + 2], line));
        ++d->rows;
    }
    return {std::move(train), std::move(test)};
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
    double uniform() { return static_cast<double>(next() >> 11) * 0x1.0p-53; }

private:
    std::uint64_t state_;
};

// Uniform points in the unit cube; the class is which third of [0, 2] the sum
// of the first two coordinates falls in. Matches data.py's synthetic().
inline Dataset synthetic(std::size_t rows, std::size_t cols, std::uint64_t seed)
{
    SplitMix64 rng(seed);
    Dataset d;
    d.rows = rows;
    d.cols = cols;
    d.values.resize(rows * cols);
    for (double& v : d.values)
        v = rng.uniform();
    for (std::size_t r = 0; r < rows; ++r) {
        const double s = d.row(r)[0] + d.row(r)[1];
        d.labels.push_back((s >= 2.0 / 3.0 ? 1 : 0) + (s >= 4.0 / 3.0 ? 1 : 0));
    }
    return d;
}

}  // namespace knn
