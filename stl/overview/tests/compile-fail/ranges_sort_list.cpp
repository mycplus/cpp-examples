// ranges_sort_list.cpp - must NOT compile: the same mistake with C++20 ranges.
#include <algorithm>
#include <list>

int main()
{
    std::list<int> numbers{3, 1, 4, 1, 5};
    std::ranges::sort(numbers);
}
