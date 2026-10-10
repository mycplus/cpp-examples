// hiding.cpp - a function in a derived class hides every base-class function
// with the same name, whatever their parameters.
#include <iostream>
#include <string>

struct Printer {
    void print() const { std::cout << "Printer::print()\n"; }
    void print(const std::string& s) const { std::cout << "Printer::print(\"" << s << "\")\n"; }
};

struct ColorPrinter : Printer {
    void print(int color) const { std::cout << "ColorPrinter::print(" << color << ")\n"; }
};

struct FixedPrinter : Printer {
    using Printer::print;              // bring the base overloads back
    void print(int color) const { std::cout << "FixedPrinter::print(" << color << ")\n"; }
};

int main() {
    ColorPrinter c;
    c.print(3);
    c.Printer::print();                // the scope operator reaches the hidden one
    c.Printer::print("draft");

    FixedPrinter f;
    f.print(3);
    f.print();
    f.print("final");
}
