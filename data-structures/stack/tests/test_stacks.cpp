// Tests for ArrayStack and LinkedStack: random operations checked against
// std::stack, non-trivial and move-only element types, empty-stack errors,
// and destruction and move-assignment of a stack of a million nodes.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

#include "array_stack.hpp"
#include "linked_stack.hpp"

static int failures = 0;
static void check(bool ok, const char* expr, int line)
{
    if (!ok) {
        std::cerr << __FILE__ << ':' << line << ": CHECK failed: " << expr << '\n';
        ++failures;
    }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

static std::uint64_t rng_state = 88172645463325252ULL;
static std::uint64_t next_rand()
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}

template <typename Stack>
void differential_against_std_stack()
{
    Stack s;
    std::stack<std::string> model;
    for (int i = 0; i < 100000; ++i) {
        const std::uint64_t r = next_rand();
        if (r % 3 != 0) {
            std::string v = "value-" + std::to_string(r % 100000);
            s.push(v);
            model.push(v);
        } else {
            auto got = s.try_pop();
            CHECK(got.has_value() == !model.empty());
            if (!model.empty()) {
                CHECK(*got == model.top());
                model.pop();
            }
        }
        CHECK(s.size() == model.size());
        CHECK(s.empty() == model.empty());
        if (!model.empty())
            CHECK(s.top() == model.top());
    }
}

template <typename Stack>
void empty_stack_errors(const std::string& name)
{
    Stack s;
    CHECK(s.empty() && s.size() == 0);
    CHECK(!s.try_pop().has_value());
    bool threw = false;
    try {
        s.pop();
    } catch (const std::out_of_range& e) {
        threw = std::string(e.what()) == name + "::pop on an empty stack";
    }
    CHECK(threw);
    threw = false;
    try {
        (void)s.top();
    } catch (const std::out_of_range& e) {
        threw = std::string(e.what()) == name + "::top on an empty stack";
    }
    CHECK(threw);
}

template <typename Stack>
void move_only_elements()
{
    Stack s;
    s.push(std::make_unique<int>(1));
    s.push(std::make_unique<int>(2));
    CHECK(*s.top() == 2);
    auto p = s.try_pop();
    CHECK(p && **p == 2);
    CHECK(*s.top() == 1);
    s.pop();
    CHECK(s.empty());
}

static void linked_stack_large()
{
    constexpr int n = 1'000'000;
    {
        LinkedStack<int> s;
        for (int i = 0; i < n; ++i)
            s.push(i);
        CHECK(s.size() == static_cast<std::size_t>(n) && s.top() == n - 1);
    }                                           // destructor: must not recurse
    LinkedStack<int> a;
    LinkedStack<int> b;
    for (int i = 0; i < n; ++i)
        a.push(i);
    b.push(-1);
    a = std::move(b);                           // old chain freed iteratively
    CHECK(a.size() == 1 && a.top() == -1);
    CHECK(b.empty() && b.size() == 0);          // NOLINT: moved-from is valid
    LinkedStack<int> c(std::move(a));
    CHECK(c.size() == 1 && a.empty());          // NOLINT
}

static void linked_stack_clear()
{
    LinkedStack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    s.clear();
    CHECK(s.empty() && s.size() == 0);
    s.push(4);
    CHECK(s.size() == 1 && s.top() == 4);
}

static void array_stack_emplace()
{
    ArrayStack<std::pair<int, std::string>> s;
    auto& ref = s.emplace(7, "seven");
    CHECK(ref.first == 7 && s.top().second == "seven");
}

int main()
{
    differential_against_std_stack<ArrayStack<std::string>>();
    differential_against_std_stack<LinkedStack<std::string>>();
    empty_stack_errors<ArrayStack<int>>("ArrayStack");
    empty_stack_errors<LinkedStack<int>>("LinkedStack");
    move_only_elements<ArrayStack<std::unique_ptr<int>>>();
    move_only_elements<LinkedStack<std::unique_ptr<int>>>();
    linked_stack_large();
    linked_stack_clear();
    array_stack_emplace();
    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "stacks: all tests passed\n";
    return EXIT_SUCCESS;
}
