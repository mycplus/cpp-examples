// map_lookup.cpp - operator[] inserts a key that is not there; find() does not.
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

int main()
{
    std::map<std::string, int> stock{{"apples", 4}, {"pears", 0}};

    // Looks like a read. It is an insert when the key is missing.
    if (stock["plums"] > 0)
        std::cout << "plums in stock\n";
    std::cout << "after operator[]: " << stock.size() << " keys, including \"plums\"\n";

    // find() reports absence without changing the map.
    if (auto it = stock.find("cherries"); it == stock.end())
        std::cout << "find(\"cherries\"): not found, still " << stock.size() << " keys\n";

    // at() throws for a missing key instead of inserting.
    try {
        std::cout << stock.at("figs") << '\n';
    } catch (const std::out_of_range&) {
        std::cout << "at(\"figs\"): threw std::out_of_range\n";
    }
}
