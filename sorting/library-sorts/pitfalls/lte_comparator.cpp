// lte_comparator.cpp - std::sort with <= instead of < (undefined behavior)
#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v(100, 7);                    // 100 equal values
    std::sort(v.begin(), v.end(),
              [](int a, int b) { return a <= b; }); // not a strict weak ordering
    std::cout << "sorted " << v.size() << " values\n";
    return 0;
}
