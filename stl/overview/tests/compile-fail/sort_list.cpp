// sort_list.cpp - must NOT compile: std::sort needs random-access iterators,
// and std::list provides bidirectional ones.
#include <algorithm>
#include <list>

int main()
{
    std::list<int> numbers{3, 1, 4, 1, 5};
    std::sort(numbers.begin(), numbers.end());
}
