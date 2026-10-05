// long_list.cpp - a singly linked list whose nodes own the next node through
// std::unique_ptr. The default destructor destroys the chain recursively;
// clear() unlinks it one node at a time.
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

struct Node {
    explicit Node(int v) : value(v) {}
    int value;
    std::unique_ptr<Node> next;
};

class List {
public:
    List() = default;
    List(const List&) = delete;
    List& operator=(const List&) = delete;
    ~List() { if (iterative_) clear(); }   // otherwise head_'s destructor recurses

    explicit List(bool iterative) : iterative_(iterative) {}

    void push_front(int v)
    {
        auto node = std::make_unique<Node>(v);
        node->next = std::move(head_);
        head_ = std::move(node);
    }

    void clear() noexcept
    {
        while (head_)
            head_ = std::move(head_->next);   // the old head dies with an empty next
    }

private:
    std::unique_ptr<Node> head_;
    bool iterative_ = true;
};

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "usage: long_list <node-count> recursive|iterative\n";
        return 2;
    }
    const long count = std::strtol(argv[1], nullptr, 10);
    const bool iterative = std::string_view(argv[2]) == "iterative";

    {
        List list(iterative);
        for (long i = 0; i < count; ++i)
            list.push_front(static_cast<int>(i));
        std::cout << "built " << count << " nodes, destroying ("
                  << (iterative ? "iterative" : "recursive") << ")..." << std::endl;
    }
    std::cout << "destroyed" << std::endl;
}
