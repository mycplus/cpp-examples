// account.cpp - encapsulation: the balance can change only through
// deposit() and withdraw(), which never let it go below zero.
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Account {
public:
    Account(std::string owner, long opening) : owner_(std::move(owner)) {
        deposit(opening);
    }

    void deposit(long amount) {
        require_positive(amount);
        balance_ += amount;
        history_.push_back(amount);
    }

    void withdraw(long amount) {
        require_positive(amount);
        if (amount > balance_) {
            throw std::invalid_argument("insufficient funds: balance " + std::to_string(balance_) +
                                        ", requested " + std::to_string(amount));
        }
        balance_ -= amount;
        history_.push_back(-amount);
    }

    const std::string& owner() const { return owner_; }
    long balance() const { return balance_; }
    const std::vector<long>& history() const { return history_; }  // read-only to callers

private:
    static void require_positive(long amount) {
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive: " + std::to_string(amount));
        }
    }

    std::string owner_;
    long balance_ = 0;
    std::vector<long> history_;
};

int main() {
    Account acct("Ada", 100);
    std::cout << acct.owner() << " opened with " << acct.balance() << '\n';

    acct.deposit(50);
    std::cout << "deposit 50 -> balance " << acct.balance() << '\n';
    acct.withdraw(30);
    std::cout << "withdraw 30 -> balance " << acct.balance() << '\n';

    for (long amount : {500L, -5L}) {
        try {
            acct.withdraw(amount);
        } catch (const std::invalid_argument& e) {
            std::cout << "error: " << e.what() << '\n';
        }
    }
    std::cout << "balance " << acct.balance() << '\n';

    std::cout << "history:";
    for (long t : acct.history()) {
        std::cout << ' ' << (t > 0 ? "+" : "") << t;
    }
    std::cout << '\n';

    // acct.balance_ = 1000000;  // does not compile: balance_ is private
}
