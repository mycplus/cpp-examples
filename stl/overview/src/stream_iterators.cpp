// stream_iterators.cpp - algorithms reading from and writing to streams.
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

int main()
{
    std::istringstream input("10 20 3 45 15 8 30");   // stands in for a file or std::cin

    // Read every int until end of input; a default-constructed istream_iterator
    // compares equal to one that has reached end of stream.
    std::vector<int> values(std::istream_iterator<int>(input),
                            std::istream_iterator<int>{});

    // Write straight to std::cout: no output loop.
    std::cout << "all:      ";
    std::copy(values.begin(), values.end(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << "\nat most 15: ";
    std::remove_copy_if(values.begin(), values.end(),
                        std::ostream_iterator<int>(std::cout, " "),
                        [](int x) { return x > 15; });

    // back_inserter calls push_back, so the algorithm grows the destination.
    std::vector<int> doubled;
    std::transform(values.begin(), values.end(), std::back_inserter(doubled),
                   [](int x) { return x * 2; });
    std::cout << "\ndoubled:  ";
    std::copy(doubled.begin(), doubled.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\nsizes: values " << values.size() << ", doubled " << doubled.size() << '\n';
}
