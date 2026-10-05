// two_owners.cpp - two independent shared_ptrs built from one raw pointer.
// Each has its own control block, so each deletes the object.
#include <iostream>
#include <memory>

struct Session {
    ~Session() { std::cout << "  ~Session\n"; }
};

int main()
{
    Session* raw = new Session;
    std::shared_ptr<Session> a(raw);
    std::shared_ptr<Session> b(raw);      // second control block for the same object
    std::cout << "  a.use_count() = " << a.use_count()
              << ", b.use_count() = " << b.use_count() << '\n';
}
