// shipping.cpp - the object-oriented version: each shipping method is an
// object that knows its own cost, and quote() works with any of them.
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class ShippingMethod {  // abstraction: what every shipping method offers
public:
    virtual ~ShippingMethod() = default;
    virtual std::string name() const = 0;
    virtual long cost_cents(int kg) const = 0;
};

class RateShipping : public ShippingMethod {  // inheritance: one implementation
public:
    RateShipping(std::string name, long base_cents, long per_kg_cents)
        : name_(std::move(name)), base_(base_cents), per_kg_(per_kg_cents) {
        if (base_ < 0 || per_kg_ < 0) throw std::invalid_argument("rates must not be negative");
    }
    std::string name() const override { return name_; }
    long cost_cents(int kg) const override { return base_ + per_kg_ * kg; }

private:  // encapsulation: the rates are checked once and can't change afterwards
    std::string name_;
    long base_;
    long per_kg_;
};

class Pickup : public ShippingMethod {
public:
    std::string name() const override { return "pickup"; }
    long cost_cents(int) const override { return 0; }
};

class Courier : public ShippingMethod {  // added later: quote() did not change
public:
    std::string name() const override { return "courier"; }
    long cost_cents(int) const override { return 750; }
};

std::string money(long cents) {
    std::string fraction = std::to_string(cents % 100);
    return std::to_string(cents / 100) + "." + (fraction.size() < 2 ? "0" : "") + fraction;
}

void quote(const ShippingMethod& method, int kg) {  // polymorphism: any method works
    std::cout << std::left << std::setw(8) << method.name() << ' ' << kg
              << " kg: " << money(method.cost_cents(kg)) << '\n';
}

int main() {
    std::vector<std::unique_ptr<ShippingMethod>> methods;
    methods.push_back(std::make_unique<RateShipping>("standard", 400, 50));
    methods.push_back(std::make_unique<RateShipping>("express", 1000, 100));
    methods.push_back(std::make_unique<Pickup>());
    methods.push_back(std::make_unique<Courier>());

    for (const auto& method : methods) quote(*method, 2);
}
