// knn_bench.cpp - time brute-force KNN prediction on the synthetic data that
// the Python benchmark (benchmark.py) uses: 20,000 training rows, 1,000
// queries, 8 features, k = 5, one thread. The checksum must match Python's.
//
// Usage: knn_bench [repeats]   (default 5; the median time is reported)
#include "knn.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv)
{
    const int repeats = argc > 1 ? std::atoi(argv[1]) : 5;
    if (repeats < 1) {
        std::fprintf(stderr, "usage: knn_bench [repeats >= 1]\n");
        return 2;
    }
    const knn::Dataset train = knn::synthetic(20000, 8, 1);
    const knn::Dataset queries = knn::synthetic(1000, 8, 2);
    constexpr std::size_t k = 5;

    std::vector<int> pred;
    std::vector<double> seconds;
    for (int i = 0; i < repeats; ++i) {
        const auto start = std::chrono::steady_clock::now();
        pred = knn::predict(train, queries, k);
        const std::chrono::duration<double> took = std::chrono::steady_clock::now() - start;
        seconds.push_back(took.count());
    }
    std::sort(seconds.begin(), seconds.end());

    long long checksum = 0;
    int right = 0;
    for (std::size_t i = 0; i < pred.size(); ++i) {
        checksum += static_cast<long long>(i + 1) * pred[i];
        right += pred[i] == queries.labels[i];
    }
    std::printf("%zu training rows, %zu queries, %zu features, k=%zu\n", train.rows,
                queries.rows, train.cols, k);
    std::printf("checksum of predictions: %lld; accuracy %.3f\n", checksum,
                right / static_cast<double>(queries.rows));
    std::printf("microseconds per query (median of %d run%s): %.1f\n", repeats,
                repeats == 1 ? "" : "s",
                1e6 * seconds[seconds.size() / 2] / static_cast<double>(queries.rows));
    return 0;
}
