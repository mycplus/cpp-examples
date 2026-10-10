// private_member.cpp - must not compile: a derived class cannot read its
// base class's private members.
class Base {
private:
    int secret = 42;
};

class Derived : public Base {
public:
    int peek() const { return secret; }
};

int main() { return Derived{}.peek(); }
