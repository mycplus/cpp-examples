// cast_cost.cpp - rough cost of a virtual call, a typeid comparison and
// dynamic_cast, in nanoseconds per operation. Build with optimization on.
// The numbers depend on the compiler, the CPU and the class hierarchy; use
// them to compare the operations with each other, not as absolute figures.
#include <chrono>
#include <cstdio>
#include <memory>
#include <typeinfo>
#include <vector>

struct Base { virtual ~Base() = default; virtual int value() const { return 1; } };
struct D1 : Base { int value() const override { return 2; } };
struct D2 : D1 {};
struct D3 : D2 {};
struct D4 : D3 {};
struct D5 : D4 {};                               // five levels below Base
struct Other : Base {};
struct Mixin { virtual ~Mixin() = default; };
struct Multi : Base, Mixin {};                   // for the cross-cast

template <class F>
double ns_per_op(const std::vector<Base*>& objects, F op)
{
    constexpr int rounds = 2000;
    volatile long sink = 0;
    const auto start = std::chrono::steady_clock::now();
    for (int r = 0; r < rounds; ++r)
        for (Base* b : objects)
            sink = sink + op(b);
    const auto end = std::chrono::steady_clock::now();
    const double ns = std::chrono::duration<double, std::nano>(end - start).count();
    return ns / (static_cast<double>(rounds) * static_cast<double>(objects.size()));
}

int main()
{
    std::vector<std::unique_ptr<Base>> owned;
    std::vector<Base*> deep, multi;
    for (int i = 0; i < 5000; ++i) {
        owned.push_back(std::make_unique<D5>());
        deep.push_back(owned.back().get());
        owned.push_back(std::make_unique<Multi>());
        multi.push_back(owned.back().get());
    }
    std::printf("virtual call                       %6.2f ns\n",
                ns_per_op(deep, [](Base* b) { return b->value(); }));
    std::printf("typeid(*b) == typeid(D5)           %6.2f ns\n",
                ns_per_op(deep, [](Base* b) { return typeid(*b) == typeid(D5) ? 1 : 0; }));
    std::printf("dynamic_cast<D5*> (exact type)     %6.2f ns\n",
                ns_per_op(deep, [](Base* b) { return dynamic_cast<D5*>(b) ? 1 : 0; }));
    std::printf("dynamic_cast<D1*> (4 levels up)    %6.2f ns\n",
                ns_per_op(deep, [](Base* b) { return dynamic_cast<D1*>(b) ? 1 : 0; }));
    std::printf("dynamic_cast<Other*> (fails)       %6.2f ns\n",
                ns_per_op(deep, [](Base* b) { return dynamic_cast<Other*>(b) ? 1 : 0; }));
    std::printf("dynamic_cast<Mixin*> (cross-cast)  %6.2f ns\n",
                ns_per_op(multi, [](Base* b) { return dynamic_cast<Mixin*>(b) ? 1 : 0; }));
}
