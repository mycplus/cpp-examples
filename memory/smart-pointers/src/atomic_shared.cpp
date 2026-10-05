// atomic_shared.cpp - C++20: std::atomic<std::shared_ptr<T>> makes concurrent
// loads and stores of one shared_ptr object safe.
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

int main()
{
    std::atomic<std::shared_ptr<int>> config{std::make_shared<int>(1)};

    std::thread writer1([&] { for (int i = 0; i < 1000; ++i) config.store(std::make_shared<int>(i)); });
    std::thread writer2([&] { for (int i = 0; i < 1000; ++i) config.store(std::make_shared<int>(-i)); });
    writer1.join();
    writer2.join();

    std::shared_ptr<int> snapshot = config.load();
    std::cout << "atomic<shared_ptr>: done, last value is "
              << (*snapshot == 999 || *snapshot == -999 ? "from a final store" : "unexpected")
              << ", is_lock_free() = " << std::boolalpha << config.is_lock_free() << '\n';
}
