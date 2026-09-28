// array_stack.hpp - a stack adapter over std::vector's contiguous storage.
#ifndef MYCPLUS_ARRAY_STACK_HPP
#define MYCPLUS_ARRAY_STACK_HPP

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class ArrayStack {
public:
    void push(const T& value) { items_.push_back(value); }
    void push(T&& value) { items_.push_back(std::move(value)); }

    template <typename... Args>
    T& emplace(Args&&... args)
    {
        return items_.emplace_back(std::forward<Args>(args)...);
    }

    T& top()             { require_non_empty("top"); return items_.back(); }
    const T& top() const { require_non_empty("top"); return items_.back(); }

    void pop() { require_non_empty("pop"); items_.pop_back(); }

    // Moves the top element out, or returns std::nullopt on an empty stack.
    std::optional<T> try_pop()
    {
        if (items_.empty())
            return std::nullopt;
        std::optional<T> value(std::move(items_.back()));
        items_.pop_back();
        return value;
    }

    [[nodiscard]] bool empty() const noexcept { return items_.empty(); }
    std::size_t size() const noexcept { return items_.size(); }

private:
    void require_non_empty(const char* op) const
    {
        if (items_.empty())
            throw std::out_of_range(std::string("ArrayStack::") + op +
                                    " on an empty stack");
    }

    std::vector<T> items_;
};

#endif
