// missing_base_ctor.cpp - must not compile: Base has no default constructor,
// so Derived's constructor has to call one of Base's constructors.
#include <string>

class Base {
public:
    explicit Base(std::string name) : name_(name) {}
private:
    std::string name_;
};

class Derived : public Base {
public:
    Derived() {}               // which Base constructor? none can be called
};

int main() { Derived d; }
