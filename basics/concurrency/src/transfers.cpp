// transfers.cpp - moving money between two accounts from two threads in
// opposite directions. Locking both mutexes with std::scoped_lock avoids the
// deadlock that locking them one at a time, in different orders, can cause.
#include <iostream>
#include <mutex>
#include <thread>

struct Account {
    std::mutex m;
    long balance = 0;
};

void transfer(Account& from, Account& to, long amount)
{
    std::scoped_lock lock(from.m, to.m);   // locks both, deadlock-free, in any order
    from.balance -= amount;
    to.balance += amount;
}

int main()
{
    Account a, b;
    a.balance = 1000;
    b.balance = 1000;

    std::thread t1([&] { for (int i = 0; i < 100'000; ++i) transfer(a, b, 1); });
    std::thread t2([&] { for (int i = 0; i < 100'000; ++i) transfer(b, a, 1); });
    t1.join();
    t2.join();

    std::cout << "a = " << a.balance << ", b = " << b.balance
              << ", total = " << a.balance + b.balance << '\n';
}
