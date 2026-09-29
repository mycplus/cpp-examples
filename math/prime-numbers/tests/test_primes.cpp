// Tests for src/primes.cpp, included with main() renamed.
#define main primes_demo_main
#include "../src/primes.cpp"
#undef main

#include <cstdlib>
#include <limits>

namespace {

int failures = 0;
void check(bool ok, const char* expr, int line)
{
    if (!ok) {
        std::cerr << __FILE__ << ':' << line << ": CHECK failed: " << expr << '\n';
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

void test_small_and_edge_values()
{
    const std::int64_t primes[] = {2, 3, 5, 7, 11, 13, 97, 7919, 1000003,
                                   2147483647, 1000000007, 999999999989};
    const std::int64_t composites[] = {std::numeric_limits<std::int64_t>::min(), -7, -1, 0, 1,
                                       4, 6, 9, 25, 49, 91, 121, 561, 1105, 7917,
                                       std::int64_t{1000003} * 1000003,   // prime squared
                                       std::int64_t{1000003} * 1000033,   // two large primes
                                       std::int64_t{2147483647} * 2};
    for (std::int64_t p : primes)
        CHECK(is_prime(p));
    for (std::int64_t c : composites)
        CHECK(!is_prime(c));
}

void test_trial_division_matches_sieve()
{
    const std::size_t limit = 200000;
    const std::vector<bool> flags = sieve(limit);
    for (std::size_t k = 0; k < limit; ++k)
        if (is_prime(static_cast<std::int64_t>(k)) != flags[k]) {
            std::cerr << "disagree at " << k << '\n';
            CHECK(!"trial division and sieve disagree");
            break;
        }
}

void test_prime_counts()                       // published values of pi(x)
{
    CHECK(count_primes_below(0) == 0 && count_primes_below(1) == 0);
    CHECK(count_primes_below(2) == 0 && count_primes_below(3) == 1);
    CHECK(count_primes_below(10) == 4 && count_primes_below(100) == 25);
    CHECK(count_primes_below(1000) == 168);
    CHECK(count_primes_below(1000000) == 78498);
    CHECK(count_primes_below(10000000) == 664579);
}

}  // namespace

int main()
{
    test_small_and_edge_values();
    test_trial_division_matches_sieve();
    test_prime_counts();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "primes: all tests passed\n";
    return EXIT_SUCCESS;
}
