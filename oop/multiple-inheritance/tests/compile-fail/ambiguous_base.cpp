// ambiguous_base.cpp - must not compile: without virtual inheritance a Copier
// contains two Device objects, so Copier* cannot convert to Device*.
struct Device  { int id = 0; };
struct Scanner : Device {};
struct Printer : Device {};
struct Copier  : Scanner, Printer {};

int main() {
    Copier c;
    Device* d = &c;
    return d->id;
}
