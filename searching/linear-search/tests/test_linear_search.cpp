// test_linear_search.cpp - checks find_index() and move_to_front() against
// a reference that scans backwards, on vectors and lists of random values.
// Includes the program's source so the listing stays one complete file.
#define main linear_search_program_main
#include "../src/linear_search.cpp"
#undef main

#include <cstdint>
#include <cstdio>

namespace {

int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::printf("FAILED line %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

std::uint64_t rng_state = 2026;
std::uint64_t next_random()  // splitmix64
{
    std::uint64_t z = (rng_state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
int below(int n) { return static_cast<int>(next_random() % static_cast<std::uint64_t>(n)); }

std::optional<std::size_t> reference(const std::vector<int>& v, int key)
{
    std::optional<std::size_t> first;
    for (std::size_t k = v.size(); k-- > 0;)
        if (v[k] == key)
            first = k;
    return first;
}

}  // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    CHECK(!find_index(std::vector<int>{}, 1));
    CHECK(find_index(std::string("hello"), 'l') == std::optional<std::size_t>(2));
    std::vector<int> one{5};
    CHECK(move_to_front(one, 5) == std::optional<std::size_t>(0) && one == std::vector<int>{5});

    int agree = 0;
    const int trials = 20000;
    for (int t = 0; t < trials; ++t) {
        std::vector<int> v(static_cast<std::size_t>(below(65)));
        const int range = 1 + below(40);
        for (int& x : v)
            x = below(range) - range / 2;
        const int key = below(range + 4) - range / 2 - 2;
        const auto want = reference(v, key);
        const std::list<int> as_list(v.begin(), v.end());

        bool ok = find_index(v, key) == want && find_index(as_list, key) == want;
        std::vector<int> moved = v;
        ok = ok && move_to_front(moved, key) == want;
        std::vector<int> expect = v;
        if (want) {
            expect.erase(expect.begin() + static_cast<std::ptrdiff_t>(*want));
            expect.insert(expect.begin(), key);
        }
        ok = ok && moved == expect;
        CHECK(ok);
        agree += ok;
    }
    std::printf("random vectors and lists matching the reference: %d of %d\n", agree, trials);
    std::printf("%d failed\n", failures);
    return failures != 0;
}
