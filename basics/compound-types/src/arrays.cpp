// arrays.cpp - an array's type includes its length. The length is lost when
// the array decays to a pointer, and kept by a reference to the array or by
// std::array.
#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <type_traits>

// Receives only an address, so the caller must pass the count separately.
double total_by_pointer(const double* prices, std::size_t count)
{
    double sum = 0;
    for (std::size_t i = 0; i < count; ++i)
        sum += prices[i];
    return sum;
}

// A reference to an array keeps the length in the type: N is deduced.
template <std::size_t N>
double total_by_reference(const double (&prices)[N])
{
    double sum = 0;
    for (double p : prices)
        sum += p;
    return sum;
}

// std::array is a class wrapping a C array: it can be passed, copied and
// returned like any other value, and it always knows its size.
double total_of(const std::array<double, 4>& basket)
{
    double sum = 0;
    for (double p : basket)
        sum += p;
    return sum;
}

int main()
{
    std::cout << std::fixed << std::setprecision(2);

    double prices[] = {19.99, 5.49, 3.75, 12.00};
    static_assert(std::is_same_v<decltype(prices), double[4]>, "the length is part of the type");
    std::cout << "sizeof(prices) = " << sizeof(prices) << " bytes, "
              << std::size(prices) << " elements\n";

    const double* first = prices;   // the array decays to a pointer to its first element
    std::cout << "sizeof(first) = " << sizeof(first) << " bytes: only the address is left\n";

    std::cout << "total by pointer and count:  " << total_by_pointer(prices, std::size(prices)) << '\n';
    std::cout << "total by reference to array: " << total_by_reference(prices) << '\n';

    std::array<double, 4> basket{19.99, 5.49, 3.75, 12.00};
    std::array<double, 4> copy = basket;    // a C array cannot be copied with =
    copy[0] = 0;
    std::cout << "std::array: size() = " << basket.size() << ", total = " << total_of(basket)
              << ", copy total = " << total_of(copy) << '\n';
}
