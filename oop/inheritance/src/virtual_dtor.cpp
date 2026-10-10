// virtual_dtor.cpp - deleting a derived object through a base pointer.
// Without a virtual destructor in the base, this is undefined behavior.
#include <iostream>
#include <memory>
#include <string>

struct Plain {                         // no virtual destructor
    ~Plain() { std::cout << "  ~Plain\n"; }
};
struct PlainChild : Plain {
    std::string buffer = std::string(64, 'x');   // owns heap memory
    ~PlainChild() { std::cout << "  ~PlainChild\n"; }
};

struct Safe {
    virtual ~Safe() { std::cout << "  ~Safe\n"; }
};
struct SafeChild : Safe {
    std::string buffer = std::string(64, 'x');
    ~SafeChild() override { std::cout << "  ~SafeChild\n"; }
};

int main(int argc, char*[]) {
    std::cout << "virtual destructor:\n";
    { std::unique_ptr<Safe> p = std::make_unique<SafeChild>(); }

    if (argc > 1) {                    // the broken case runs only on request
        std::cout << "no virtual destructor:\n";
        Plain* p = new PlainChild;
        delete p;                      // undefined behavior
    }
}
