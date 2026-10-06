// first_look.cpp - a container, iterators and algorithms working together.
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

int main()
{
    std::vector<std::string> words{"iterator", "map", "algorithm", "set",
                                   "vector", "deque", "list", "array"};

    std::sort(words.begin(), words.end());                    // algorithm + iterators

    auto long_words = std::count_if(words.begin(), words.end(),
                                    [](const std::string& w) { return w.size() > 5; });

    std::size_t letters = std::accumulate(words.begin(), words.end(), std::size_t{0},
                                          [](std::size_t sum, const std::string& w) {
                                              return sum + w.size();
                                          });

    for (const auto& w : words)
        std::cout << w << ' ';
    std::cout << "\nwords longer than 5 letters: " << long_words
              << "\ntotal letters: " << letters << '\n';
}
