// default_private_base.cpp - must not compile: `class D : B` inherits
// privately, so B's public members are private in D.
class Base {
public:
    int value = 1;
};

class Derived : Base {};       // no access specifier: private inheritance

int main() {
    Derived d;
    return d.value;
}
