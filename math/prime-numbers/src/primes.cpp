// primes.cpp - test one number for primality, and list primes with the
// Sieve of Eratosthenes.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic primes.cpp -o primes
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <vector>

// Trial division by 2, 3 and then 6k - 1 and 6k + 1 up to sqrt(n).
// i <= n / i is the overflow-safe form of i * i <= n.
constexpr bool is_prime(std::int64_t n)
{
    if (n < 2)
        return false;
    if (n < 4)
        return true;                         // 2 and 3
    if (n % 2 == 0 || n % 3 == 0)
        return false;
    for (std::int64_t i = 5; i <= n / i; i += 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    return true;
}

// constexpr lets the compiler check known values while it builds.
static_assert(is_prime(2147483647), "2^31 - 1 is prime");
static_assert(!is_prime(1) && !is_prime(91), "1 and 7 * 13 are not prime");

// flags[k] is true when k is prime, for 0 <= k < limit.
std::vector<bool> sieve(std::size_t limit)
{
    std::vector<bool> flags(limit, true);
    for (std::size_t k = 0; k < limit && k < 2; ++k)
        flags[k] = false;                    // 0 and 1
    for (std::size_t p = 2; limit > 0 && p <= (limit - 1) / p; ++p) {
        if (!flags[p])
            continue;
        for (std::size_t m = p * p; m < limit; m += p)
            flags[m] = false;
    }
    return flags;
}

std::size_t count_primes_below(std::size_t limit)
{
    std::size_t count = 0;
    for (bool f : sieve(limit))
        count += f;
    return count;
}

int main()
{
    const std::vector<bool> flags = sieve(100);
    std::cout << "primes below 100:";
    for (std::size_t k = 0; k < flags.size(); ++k)
        if (flags[k])
            std::cout << ' ' << k;
    std::cout << '\n';

    std::cout << "primes below 1000: " << count_primes_below(1000) << '\n';
    std::cout << "primes below 1000000: " << count_primes_below(1000000) << '\n';

    const std::int64_t samples[] = {-7, 0, 1, 2, 91, 97};
    std::cout << "is_prime:" << std::boolalpha;
    for (std::size_t k = 0; k < std::size(samples); ++k)
        std::cout << ' ' << samples[k] << ' ' << is_prime(samples[k])
                  << (k + 1 < std::size(samples) ? "," : "\n");
    std::cout << "is_prime(2147483647) = " << is_prime(2147483647) << '\n';
    std::cout << "is_prime(1000000007) = " << is_prime(1000000007) << '\n';
    return 0;
}
