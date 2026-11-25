// tree_bench.cpp - time the exported tree on the rows the Python benchmark
// (benchmark.py) uses: 1,000,008 rows drawn from the 114 test rows in the
// shared pseudo-random order, one thread.
// Both the loaded node table and the generated nested-if function must give
// Python's checksum.
//
// Usage: tree_bench [repeats]   (default 7; the median time is reported)
#include "generated/breast_cancer_tree.hpp"
#include "tree.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>

namespace {

std::int64_t checksum(const std::vector<int>& pred)
{
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < pred.size(); ++i)
        sum = (sum + static_cast<std::int64_t>(i + 1) * pred[i]) % 1000000007;
    return sum;
}

double median_ns_per_row(int repeats, std::size_t n, const std::function<void()>& run)
{
    std::vector<double> times;
    for (int i = 0; i < repeats; ++i) {
        const auto start = std::chrono::steady_clock::now();
        run();
        const std::chrono::duration<double> took = std::chrono::steady_clock::now() - start;
        times.push_back(took.count());
    }
    std::sort(times.begin(), times.end());
    return 1e9 * times[times.size() / 2] / static_cast<double>(n);
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        const int repeats = argc > 1 ? std::atoi(argv[1]) : 7;
        if (repeats < 1) {
            std::fprintf(stderr, "usage: tree_bench [repeats >= 1]\n");
            return 2;
        }
        const dtree::Rows test = dtree::load_rows("data/breast_cancer_test.csv");
        const dtree::Tree tree = dtree::Tree::load("models/tree.txt", test.names.size());
        const std::size_t width = test.names.size();
        std::vector<double> rows;
        for (std::size_t r : dtree::benchmark_order())
            rows.insert(rows.end(), test.row(r), test.row(r) + width);
        const std::size_t n = rows.size() / width;

        std::vector<int> loaded(n), generated(n);
        const double t_loaded = median_ns_per_row(repeats, n, [&] {
            for (std::size_t i = 0; i < n; ++i)
                loaded[i] = tree.predict(&rows[i * width]);
        });
        const double t_generated = median_ns_per_row(repeats, n, [&] {
            for (std::size_t i = 0; i < n; ++i)
                generated[i] = generated::breast_cancer_tree(&rows[i * width]);
        });

        std::printf("%zu rows, tree depth 3, %zu leaves\n", n, tree.leaves());
        std::printf("checksums: loaded %lld, generated %lld\n",
                    static_cast<long long>(checksum(loaded)),
                    static_cast<long long>(checksum(generated)));
        std::printf("nanoseconds per row (median of %d run%s):\n", repeats, repeats == 1 ? "" : "s");
        std::printf("  loaded node table        %6.2f\n", t_loaded);
        std::printf("  generated nested ifs     %6.2f\n", t_generated);
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "tree_bench: %s\n", e.what());
        return 1;
    }
}
