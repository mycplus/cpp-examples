// shared_from_this.cpp - an object that hands out shared_ptrs to itself.
#include <iostream>
#include <memory>
#include <vector>

class Connection : public std::enable_shared_from_this<Connection> {
public:
    // Registers this connection with a list that must keep it alive.
    void register_with(std::vector<std::shared_ptr<Connection>>& active)
    {
        active.push_back(shared_from_this());   // shares the existing control block
    }
};

int main()
{
    std::vector<std::shared_ptr<Connection>> active;

    auto conn = std::make_shared<Connection>();
    conn->register_with(active);
    std::cout << "owned by a shared_ptr: use_count = " << conn.use_count() << '\n';

    Connection on_stack;                         // not owned by any shared_ptr
    try {
        on_stack.register_with(active);
    } catch (const std::bad_weak_ptr& e) {
        std::cout << "not owned by a shared_ptr: threw std::bad_weak_ptr (" << e.what() << ")\n";
    }
}
