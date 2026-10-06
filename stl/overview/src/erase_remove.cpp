// erase_remove.cpp - std::remove does not remove anything from the container.
#include <algorithm>
#include <iostream>
#include <vector>

static void print(const char* label, const std::vector<int>& v)
{
    std::cout << label << " size " << v.size() << ":";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

int main()
{
    std::vector<int> v{1, 0, 2, 0, 3, 0, 4};
    print("start          ", v);

    auto new_end = std::remove(v.begin(), v.end(), 0);
    print("after remove   ", v);
    std::cout << "                kept elements end at index " << (new_end - v.begin()) << '\n';

    v.erase(new_end, v.end());                   // the erase-remove idiom
    print("after erase    ", v);

#if defined(__cpp_lib_erase_if)
    std::vector<int> w{1, 0, 2, 0, 3, 0, 4};
    auto removed = std::erase(w, 0);             // C++20: one call does both
    print("C++20 std::erase", w);
    std::cout << "                removed " << removed << " elements\n";
#endif
}
