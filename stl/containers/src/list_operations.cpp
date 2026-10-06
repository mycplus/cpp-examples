// list_operations.cpp - the operations std::list provides as member functions,
// and why each one exists instead of (or alongside) a generic algorithm.
#include <iostream>
#include <iterator>
#include <list>
#include <string>

static void print(const std::string& label, const std::list<int>& l)
{
    std::cout << label << " (size " << l.size() << "):";
    for (int x : l) std::cout << ' ' << x;
    std::cout << '\n';
}

int main()
{
    std::list<int> l{5, 3, 3, 8, 1, 3, 9};
    l.push_front(7);                         // O(1) at the front, which vector lacks
    l.push_back(2);
    print("start          ", l);

    l.remove(3);                             // erases every 3: the size changes
    print("remove(3)      ", l);

    l.remove_if([](int x) { return x > 8; });
    print("remove_if(>8)  ", l);

    std::list<int> dups{4, 4, 1, 4, 1, 1};
    dups.unique();                           // only *adjacent* duplicates
    print("unique unsorted", dups);
    dups.sort();
    dups.unique();
    print("sort + unique  ", dups);

    l.sort();                                // std::sort cannot take a list
    print("sort           ", l);

    std::list<int> more{0, 4, 6};            // must already be sorted
    l.merge(more);                           // relinks nodes; more is left empty
    print("merge          ", l);
    print("other after    ", more);

    std::list<int> tail{100, 200};
    auto it = std::next(tail.begin());       // points at 200
    l.splice(l.end(), tail, it);             // move one node between lists
    print("splice 200 in  ", l);
    print("tail after     ", tail);
    std::cout << "iterator still valid, now in l: " << *it << '\n';

    l.reverse();
    print("reverse        ", l);
}
