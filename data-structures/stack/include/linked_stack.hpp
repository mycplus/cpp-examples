// linked_stack.hpp - a stack built on a singly linked list of
// std::unique_ptr-owned nodes.
#ifndef MYCPLUS_LINKED_STACK_HPP
#define MYCPLUS_LINKED_STACK_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

template <typename T>
class LinkedStack {
    struct Node {
        explicit Node(T v) : value(std::move(v)) {}
        T value;
        std::unique_ptr<Node> next;   // the node below this one
    };

public:
    LinkedStack() = default;
    ~LinkedStack() { clear(); }

    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;

    LinkedStack(LinkedStack&& other) noexcept
        : head_(std::move(other.head_)), size_(std::exchange(other.size_, 0)) {}

    LinkedStack& operator=(LinkedStack&& other) noexcept
    {
        if (this != &other) {
            clear();                  // not head_ = ...: that would recurse
            head_ = std::move(other.head_);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }

    void push(T value)
    {
        auto node = std::make_unique<Node>(std::move(value));  // may throw
        node->next = std::move(head_);                          // cannot throw
        head_ = std::move(node);
        ++size_;
    }

    T& top()             { require_non_empty("top"); return head_->value; }
    const T& top() const { require_non_empty("top"); return head_->value; }

    void pop()
    {
        require_non_empty("pop");
        head_ = std::move(head_->next);
        --size_;
    }

    std::optional<T> try_pop()
    {
        if (!head_)
            return std::nullopt;
        std::optional<T> value(std::move(head_->value));
        head_ = std::move(head_->next);
        --size_;
        return value;
    }

    // Unlinks one node at a time, so destroying a long stack does not
    // recurse once per node.
    void clear() noexcept
    {
        while (head_)
            head_ = std::move(head_->next);
        size_ = 0;
    }

    [[nodiscard]] bool empty() const noexcept { return !head_; }
    std::size_t size() const noexcept { return size_; }

private:
    void require_non_empty(const char* op) const
    {
        if (!head_)
            throw std::out_of_range(std::string("LinkedStack::") + op +
                                    " on an empty stack");
    }

    std::unique_ptr<Node> head_;
    std::size_t size_ = 0;
};

#endif
