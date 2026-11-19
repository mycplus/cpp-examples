// construction.cpp - during a constructor or destructor, an object's
// dynamic type is the class whose constructor or destructor is running.
#include "type_name.h"

#include <iostream>
#include <typeinfo>

struct Widget {
    Widget() { report("Widget constructor"); }
    virtual ~Widget() { report("Widget destructor"); }
    void report(const char* where) const
    {
        std::cout << where << ": typeid(*this) is " << readable_name(typeid(*this))
                  << ", kind() says " << kind() << '\n';
    }
    virtual const char* kind() const { return "widget"; }
};

struct Button : Widget {
    Button() { report("Button constructor"); }
    ~Button() override { report("Button destructor"); }
    const char* kind() const override { return "button"; }
};

int main()
{
    Button button;
    button.report("main");
}
