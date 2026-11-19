// check.h - a minimal test helper: CHECK prints every failure and the test
// program returns the number of failures.
#pragma once
#include <iostream>

inline int g_failures = 0;

#define CHECK(condition)                                                              \
    do {                                                                              \
        if (!(condition)) {                                                           \
            std::cout << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #condition "\n"; \
            ++g_failures;                                                             \
        }                                                                             \
    } while (false)

#define CHECK_EQ(actual, expected)                                                    \
    do {                                                                              \
        const auto& a_ = (actual);                                                    \
        const auto& e_ = (expected);                                                  \
        if (!(a_ == e_)) {                                                            \
            std::cout << __FILE__ << ":" << __LINE__ << ": " #actual " is \"" << a_   \
                      << "\", expected \"" << e_ << "\"\n";                           \
            ++g_failures;                                                             \
        }                                                                             \
    } while (false)
