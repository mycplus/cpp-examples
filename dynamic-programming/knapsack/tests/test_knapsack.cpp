// Tests for src/knapsack.cpp. The source is included directly so the
// article's listing stays one complete program; its main() is renamed here.
#define main knapsack_demo_main
#include "../src/knapsack.cpp"
#undef main

#include <cstdint>
#include <cstdlib>

namespace {

int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::cerr << "FAIL line " << line << ": " << what << '\n';
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

long long brute_force(const std::vector<Item>& items, int capacity)
{
    long long best = 0;
    const std::size_t n = items.size();
    for (std::uint32_t mask = 0; mask < (1u << n); ++mask) {
        long long w = 0, v = 0;
        for (std::size_t i = 0; i < n; ++i)
            if (mask & (1u << i)) {
                w += items[i].weight;
                v += items[i].value;
            }
        if (w <= capacity && v > best)
            best = v;
    }
    return best;
}

template <class F>
bool throws_invalid(F f)
{
    try {
        f();
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

}  // namespace

int main()
{
    const std::vector<Item> example{
        {"A", 10, 60}, {"B", 20, 100}, {"C", 30, 120}, {"D", 5, 50},
    };
    const Selection s = knapsack(example, 50);
    CHECK(s.value == 230);
    CHECK((s.chosen == std::vector<std::size_t>{0, 2, 3}));

    CHECK(knapsack({}, 10).value == 0);
    CHECK(knapsack({{"X", 7, 40}}, 0).value == 0);
    CHECK(knapsack({{"X", 7, 40}}, 7).value == 40);
    CHECK(knapsack({{"Z", 0, 9}, {"Y", 3, 5}}, 0).value == 9);
    CHECK(throws_invalid([] { knapsack({{"N", -1, 5}}, 10); }));
    CHECK(throws_invalid([] { knapsack({{"X", 1, 1}}, -1); }));
    CHECK(throws_invalid([] {
        knapsack({{"H1", 1, std::numeric_limits<long long>::max()}, {"H2", 1, 1}}, 2);
    }));

    std::uint64_t state = 88172645463325252ULL;
    auto rnd = [&state](unsigned bound) {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return static_cast<unsigned>(state % bound);
    };

    const int rounds = 20000;
    for (int r = 0; r < rounds && failures <= 10; ++r) {
        std::vector<Item> items(rnd(15));
        const int capacity = static_cast<int>(rnd(101));
        for (Item& it : items)
            it = {"r", static_cast<int>(rnd(41)), static_cast<long long>(rnd(1001))};

        const Selection got = knapsack(items, capacity);
        CHECK(got.value == brute_force(items, capacity));
        long long w = 0, v = 0;
        for (std::size_t i : got.chosen) {
            w += items[i].weight;
            v += items[i].value;
        }
        CHECK(w <= capacity);
        CHECK(v == got.value);
    }

    if (failures) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "All tests passed (" << rounds
              << " random instances checked against brute force)\n";
}
