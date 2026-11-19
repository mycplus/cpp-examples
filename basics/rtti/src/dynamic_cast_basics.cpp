// dynamic_cast_basics.cpp - downcasts that succeed and fail, the reference
// form that throws, cross-casts between sibling bases, and casting to void*.
#include <iostream>
#include <memory>
#include <typeinfo>

struct Account {
    virtual ~Account() = default;
};
struct Savings : Account {
    double rate = 0.025;
};
struct Checking : Account {};

// Two unrelated interfaces, and a class that implements both.
struct Printable {
    virtual ~Printable() = default;
};
struct Serializable {
    virtual ~Serializable() = default;
};
struct Report : Printable, Serializable {};

int main()
{
    std::unique_ptr<Account> a = std::make_unique<Savings>();
    std::unique_ptr<Account> b = std::make_unique<Checking>();

    // Pointer form: nullptr when the object is not of the requested type.
    if (auto* s = dynamic_cast<Savings*>(a.get()))
        std::cout << "a is Savings, rate " << s->rate << '\n';
    if (dynamic_cast<Savings*>(b.get()) == nullptr)
        std::cout << "b is not Savings: dynamic_cast returned nullptr\n";

    // Reference form: there is no null reference, so failure throws.
    try {
        Savings& s = dynamic_cast<Savings&>(*b);
        std::cout << s.rate << '\n';
    } catch (const std::bad_cast&) {
        std::cout << "dynamic_cast<Savings&>(*b) threw std::bad_cast\n";
    }

    // Cross-cast: from one base to a sibling base of the same object.
    Report report;
    Printable* p = &report;
    Serializable* sz = dynamic_cast<Serializable*>(p);
    std::cout << "cross-cast Printable* -> Serializable*: " << (sz != nullptr ? "ok" : "failed") << '\n';

    // The two base subobjects live at different addresses inside Report.
    // dynamic_cast<void*> finds the start of the whole object from either one.
    const void* from_p = dynamic_cast<void*>(p);
    const void* from_sz = dynamic_cast<void*>(sz);
    std::cout << "both bases lead to the same object: " << std::boolalpha
              << (from_p == from_sz && from_p == static_cast<void*>(&report)) << '\n';
    std::cout << "Printable* and Serializable* hold the same address: "
              << (static_cast<void*>(p) == static_cast<void*>(sz)) << '\n';
}
