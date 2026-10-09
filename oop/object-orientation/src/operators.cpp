// operators.cpp - operator overloading that behaves like the built-in
// operators, and one that cannot: an overloaded && evaluates both operands.
#include <iostream>
#include <ostream>

class Money {
public:
    explicit Money(long cents) : cents_(cents) {}
    Money operator+(Money other) const { return Money(cents_ + other.cents_); }
    bool operator==(Money other) const { return cents_ == other.cents_; }
    friend std::ostream& operator<<(std::ostream& os, Money m)
    {
        return os << '$' << m.cents_ / 100 << '.' << (m.cents_ % 100) / 10 << m.cents_ % 10;
    }
private:
    long cents_;
};

struct Check {
    bool ok;
    Check operator&&(Check other) const { return Check{ok && other.ok}; }
};

Check check(const char* what, bool result)
{
    std::cout << "  evaluated " << what << '\n';
    return Check{result};
}

int main()
{
    Money price(1999), tax(160);
    std::cout << "price + tax = " << price + tax << '\n';
    std::cout << "equal to $21.59: " << std::boolalpha << (price + tax == Money(2159)) << '\n';

    std::cout << "built-in &&:\n";
    bool b = check("left", false).ok && check("right", true).ok;
    std::cout << "overloaded &&:\n";
    Check c = check("left", false) && check("right", true);
    std::cout << "results: " << b << ' ' << c.ok << '\n';
}
