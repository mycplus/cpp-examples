// boost_multiprecision.cpp - integers wider than any built-in type with
// Boost.Multiprecision, which has no equivalent in the C++ standard library.
// Header-only: no Boost library needs to be linked.
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <iostream>
#include <limits>

int main()
{
    using boost::multiprecision::cpp_int;

    cpp_int factorial = 1;
    for (int i = 2; i <= 30; ++i)
        factorial *= i;
    std::cout << "30! = " << factorial << '\n';

    std::cout << "largest uint64_t = " << std::numeric_limits<std::uint64_t>::max() << '\n';
    cpp_int two_128 = cpp_int(1) << 128;
    std::cout << "2^128 = " << two_128 << '\n';
    std::cout << "2^128 needs " << msb(two_128) + 1 << " bits\n";
}
