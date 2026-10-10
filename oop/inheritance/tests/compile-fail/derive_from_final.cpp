// derive_from_final.cpp - must not compile: a final class cannot be a base.
struct Payment {
    virtual ~Payment() = default;
};

struct BankTransfer final : Payment {};

struct Wire : BankTransfer {};

int main() {}
