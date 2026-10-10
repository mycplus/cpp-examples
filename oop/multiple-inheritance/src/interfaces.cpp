// interfaces.cpp - one class implementing two independent interfaces.
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

class Printable {
public:
    virtual ~Printable() = default;
    virtual std::string render() const = 0;
};

class Serializable {
public:
    virtual ~Serializable() = default;
    virtual std::string to_json() const = 0;
};

class Invoice : public Printable, public Serializable {
public:
    Invoice(std::string id, double total) : id_(std::move(id)), total_(total) {}
    std::string render() const override { return "Invoice " + id_ + ": " + money(); }
    std::string to_json() const override {
        return "{\"id\":\"" + id_ + "\",\"total\":" + money() + "}";
    }

private:
    std::string money() const {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2) << total_;
        return out.str();
    }
    std::string id_;
    double total_;
};

void print(const Printable& p)      { std::cout << "print:  " << p.render() << '\n'; }
void save(const Serializable& s)    { std::cout << "save:   " << s.to_json() << '\n'; }

int main() {
    Invoice inv("A-17", 249.5);
    print(inv);
    save(inv);

    // The two base subobjects live at different addresses inside one object.
    auto addr = [](const void* p) { return reinterpret_cast<std::uintptr_t>(p); };
    const Printable* as_printable = &inv;
    const Serializable* as_serializable = &inv;
    std::cout << "Printable part at offset    " << addr(as_printable) - addr(&inv) << '\n';
    std::cout << "Serializable part at offset " << addr(as_serializable) - addr(&inv) << '\n';
}
