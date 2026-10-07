// tbb_parallel.cpp - oneTBB's parallel_reduce and parallel_sort, checked
// against the serial standard library result. Pass --time to also print
// how long each sort took on this machine.
#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/info.h>
#include <oneapi/tbb/parallel_reduce.h>
#include <oneapi/tbb/parallel_sort.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

int main(int argc, char** argv)
{
    const bool show_time = argc > 1 && std::strcmp(argv[1], "--time") == 0;
    const std::size_t n = 10'000'000;

    std::vector<std::uint32_t> data(n);
    std::mt19937 rng(42);                              // fixed seed: same data every run
    for (auto& v : data)
        v = rng();

    const std::uint64_t serial = std::accumulate(data.begin(), data.end(), std::uint64_t{0});
    const std::uint64_t parallel = tbb::parallel_reduce(
        tbb::blocked_range<std::size_t>(0, n), std::uint64_t{0},
        [&](const tbb::blocked_range<std::size_t>& r, std::uint64_t sum) {
            for (std::size_t i = r.begin(); i != r.end(); ++i)
                sum += data[i];
            return sum;
        },
        std::plus<>());
    std::cout << "sum matches std::accumulate: " << std::boolalpha << (serial == parallel) << '\n';

    auto a = data, b = data;
    auto t0 = std::chrono::steady_clock::now();
    std::sort(a.begin(), a.end());
    auto t1 = std::chrono::steady_clock::now();
    tbb::parallel_sort(b.begin(), b.end());
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "parallel_sort matches std::sort: " << (a == b) << '\n';

    if (show_time) {
        using ms = std::chrono::duration<double, std::milli>;
        std::cout << "threads available: " << tbb::info::default_concurrency() << '\n'
                  << "std::sort      " << ms(t1 - t0).count() << " ms\n"
                  << "parallel_sort  " << ms(t2 - t1).count() << " ms\n";
    }
}
