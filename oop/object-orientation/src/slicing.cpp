// slicing.cpp - copying a derived object into a base-class object keeps only
// the base part. Polymorphism needs a reference or a pointer.
#include <iostream>
#include <string>
#include <vector>

struct Account {
    virtual ~Account() = default;
    virtual std::string describe() const { return "account"; }
    double balance = 0;
};

struct Savings : Account {
    std::string describe() const override { return "savings account"; }
    double rate = 0.045;            // a member the base class does not have
};

void by_value(Account a)            { std::cout << "  by value:     " << a.describe() << '\n'; }
void by_reference(const Account& a) { std::cout << "  by reference: " << a.describe() << '\n'; }

int main()
{
    Savings s;
    by_value(s);                    // copies the Account part only
    by_reference(s);

    std::vector<Account> accounts;  // stores Account objects, not Savings
    accounts.push_back(s);
    std::cout << "  in vector<Account>: " << accounts[0].describe() << '\n';
    std::cout << "  sizeof(Account) = " << sizeof(Account)
              << ", sizeof(Savings) = " << sizeof(Savings) << '\n';
}
