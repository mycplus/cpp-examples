// exception_safety.cpp - a constructor that throws after its first allocation.
// The destructor of a partly constructed object never runs, so only members
// that release themselves are cleaned up.
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

struct Part {
    explicit Part(std::string n, bool fail = false) : name(std::move(n))
    {
        if (fail)
            throw std::runtime_error("cannot create " + name);
        std::cout << "  create  " << name << '\n';
    }
    ~Part() { std::cout << "  destroy " << name << '\n'; }
    std::string name;
};

class RawEngine {                     // owns its parts through raw pointers
public:
    RawEngine() : pump_(new Part("pump")), valve_(new Part("valve", true)) {}
    ~RawEngine() { delete valve_; delete pump_; }   // never reached here
    RawEngine(const RawEngine&) = delete;
    RawEngine& operator=(const RawEngine&) = delete;
private:
    Part* pump_;
    Part* valve_;
};

class OwnedEngine {                   // owns its parts through unique_ptr members
public:
    OwnedEngine()
        : pump_(std::make_unique<Part>("pump")),
          valve_(std::make_unique<Part>("valve", true)) {}
private:
    std::unique_ptr<Part> pump_;      // fully constructed members are destroyed
    std::unique_ptr<Part> valve_;     // even when the constructor throws
};

int main()
{
    std::cout << "RawEngine:\n";
    try { RawEngine e; } catch (const std::exception& ex) { std::cout << "  caught: " << ex.what() << '\n'; }

    std::cout << "OwnedEngine:\n";
    try { OwnedEngine e; } catch (const std::exception& ex) { std::cout << "  caught: " << ex.what() << '\n'; }
}
