// stl_ranges.cpp - the standard library: count words with std::map, then
// rank them with std::ranges::sort and a view pipeline. Needs C++20.
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

int main()
{
    const std::string text =
        "the vector grows, the map sorts, the set rejects duplicates and "
        "the algorithm header works with all of them";

    std::map<std::string, int> counts;            // ordered by key
    std::string word;
    for (char c : text + ' ') {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            word += c;
        } else if (!word.empty()) {
            ++counts[word];
            word.clear();
        }
    }

    std::vector<std::pair<std::string, int>> ranked(counts.begin(), counts.end());
    std::ranges::sort(ranked, [](const auto& a, const auto& b) {
        return a.second != b.second ? a.second > b.second : a.first < b.first;
    });

    std::cout << counts.size() << " distinct words\n";
    for (const auto& [w, n] : ranked | std::views::take(3))
        std::cout << w << ' ' << n << '\n';

    auto long_words = counts | std::views::keys
                             | std::views::filter([](const std::string& w) { return w.size() > 6; });
    std::cout << "longer than 6 letters:";
    for (const auto& w : long_words)
        std::cout << ' ' << w;
    std::cout << '\n';
}
