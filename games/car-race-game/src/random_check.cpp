// random_check.cpp - the standard fixes std::mt19937's output but not the
// algorithm of std::uniform_int_distribution. Compare this output across
// standard libraries: the first line must match everywhere.

#include <cstdio>
#include <random>

int main()
{
    std::mt19937 a;
    for (int i = 1; i < 10000; ++i) a();
    std::printf("10000th mt19937 value: %u\n", static_cast<unsigned>(a()));

    std::mt19937 rng(1);
    std::uniform_int_distribution<int> lane(0, 2);
    std::printf("uniform_int_distribution(0, 2), seed 1:");
    for (int i = 0; i < 12; ++i) std::printf(" %d", lane(rng));
    std::printf("\nrng() %% 3, seed 1:                    ");
    std::mt19937 raw(1);
    for (int i = 0; i < 12; ++i) std::printf(" %u", static_cast<unsigned>(raw() % 3));
    std::printf("\n");
}
