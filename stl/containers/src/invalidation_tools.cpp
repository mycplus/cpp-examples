// invalidation_tools.cpp - uses an iterator or reference after the container
// has grown. Run under AddressSanitizer and under libstdc++'s debug mode
// (-D_GLIBCXX_DEBUG) to see which tool reports which case.
//   vector           iterator after push_back reallocates     (invalid)
//   deque-reference  reference after push_back at the end      (valid)
//   deque-iterator   iterator dereferenced after push_back     (invalid)
//   deque-walk       iterator advanced after push_back         (invalid)
//   unordered        iterator after the table rehashes         (invalid)
#include <cstdio>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <vector>

int main(int argc, char** argv)
{
    const std::string_view mode = argc > 1 ? argv[1] : "";

    if (mode == "vector") {
        std::vector<int> v{1, 2, 3};
        auto it = v.begin();
        v.push_back(4);                       // capacity 3 -> reallocation
        std::printf("%d\n", *it);
    } else if (mode == "deque-reference") {
        std::deque<int> d{1, 2, 3};
        int& first = d.front();
        for (int i = 0; i < 100000; ++i) d.push_back(i);
        std::printf("%d\n", first);
    } else if (mode == "deque-iterator") {
        std::deque<int> d{1, 2, 3};
        auto it = d.begin();
        for (int i = 0; i < 100000; ++i) d.push_back(i);
        std::printf("%d\n", *it);
    } else if (mode == "deque-walk") {
        std::deque<int> d{1, 2, 3};
        auto it = d.begin();
        for (int i = 0; i < 100000; ++i) d.push_back(i);
        long sum = 0;
        for (int i = 0; i < 1000; ++i, ++it) sum += *it;
        std::printf("%ld\n", sum);
    } else if (mode == "unordered") {
        std::unordered_map<int, int> u{{1, 10}};
        auto it = u.find(1);
        const auto buckets_before = u.bucket_count();
        for (int i = 2; i < 10000; ++i) u[i] = i;
        std::printf("buckets %zu -> %zu, value %d\n", buckets_before, u.bucket_count(), it->second);
    } else {
        std::fprintf(stderr, "usage: invalidation_tools vector|deque-reference|deque-iterator|deque-walk|unordered\n");
        return 2;
    }
}
