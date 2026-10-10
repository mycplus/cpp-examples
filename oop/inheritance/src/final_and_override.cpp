// final_and_override.cpp - virtual functions, override and final.
#include <iostream>
#include <memory>
#include <vector>

class Payment {
public:
    virtual ~Payment() = default;
    virtual double fee(double amount) const { return amount * 0.02; }
    virtual const char* name() const { return "payment"; }
};

class Card : public Payment {
public:
    double fee(double amount) const override { return 0.30 + amount * 0.029; }
    const char* name() const override { return "card"; }
};

// final: no class can derive from BankTransfer.
class BankTransfer final : public Payment {
public:
    double fee(double) const override { return 1.00; }
    const char* name() const final { return "bank transfer"; }
};

int main() {
    std::vector<std::unique_ptr<Payment>> methods;
    methods.push_back(std::make_unique<Payment>());
    methods.push_back(std::make_unique<Card>());
    methods.push_back(std::make_unique<BankTransfer>());

    for (const auto& m : methods)
        std::cout << m->name() << " fee on 100.00: " << m->fee(100.0) << '\n';
}
