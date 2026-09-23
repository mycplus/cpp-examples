// test_bubble_sort.cpp - bubble_sort() against std::stable_sort, several containers
#include <cstdio>
#include <list>
#include <random>

#define main bubble_sort_demo_main   // reuse the article's file as-is
#include "../src/bubble_sort.cpp"
#undef main

int main()
{
    std::mt19937 gen(2026);
    int failures = 0;

    for (int t = 0; t < 5000; ++t) {
        std::size_t n = gen() % 65;                        // 0..64 elements
        std::vector<std::pair<int, int>> v(n);             // (key, original position)
        for (std::size_t i = 0; i < n; ++i)
            v[i] = {static_cast<int>(gen() % 5), static_cast<int>(i)};  // many ties

        auto by_key = [](const auto& a, const auto& b) { return a.first < b.first; };
        auto expected = v;
        std::stable_sort(expected.begin(), expected.end(), by_key);

        std::forward_list<std::pair<int, int>> fl(v.begin(), v.end());
        std::list<std::pair<int, int>> li(v.begin(), v.end());
        bubble_sort(v.begin(), v.end(), by_key);
        bubble_sort(fl.begin(), fl.end(), by_key);
        bubble_sort(li.begin(), li.end(), by_key);

        bool ok = v == expected
               && std::equal(fl.begin(), fl.end(), expected.begin(), expected.end())
               && std::equal(li.begin(), li.end(), expected.begin(), expected.end());
        if (!ok && ++failures == 5)
            break;
    }

    std::vector<int> empty;
    bubble_sort(empty.begin(), empty.end());
    int arr[] = {3, 1, 2};                                 // plain array, raw pointers
    bubble_sort(std::begin(arr), std::end(arr));
    if (arr[0] != 1 || arr[1] != 2 || arr[2] != 3)
        ++failures;

    std::printf("stability and container tests: %d failures\n", failures);
    return failures != 0;
}
