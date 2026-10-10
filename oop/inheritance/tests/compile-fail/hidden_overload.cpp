// hidden_overload.cpp - must not compile: print(int) in the derived class
// hides Printer::print(), so the call without arguments finds no match.
struct Printer {
    void print() const {}
};

struct ColorPrinter : Printer {
    void print(int) const {}
};

int main() {
    ColorPrinter c;
    c.print();
}
