// atomic_shared.cpp - two threads replacing one shared_ptr safely.
// C++20 std::atomic<std::shared_ptr<T>> where the standard library provides
// it (__cpp_lib_atomic_shared_ptr); otherwise a mutex around a plain shared_ptr.
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

#if defined(__cpp_lib_atomic_shared_ptr)
#include <atomic>

class SharedConfig {
public:
    explicit SharedConfig(std::shared_ptr<int> p) : ptr_(std::move(p)) {}
    void store(std::shared_ptr<int> p) { ptr_.store(std::move(p)); }
    std::shared_ptr<int> load() const { return ptr_.load(); }
    static constexpr const char* how = "std::atomic<std::shared_ptr>";
private:
    std::atomic<std::shared_ptr<int>> ptr_;
};

#else
#include <mutex>

class SharedConfig {
public:
    explicit SharedConfig(std::shared_ptr<int> p) : ptr_(std::move(p)) {}
    void store(std::shared_ptr<int> p)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ptr_.swap(p);                 // the old object is released after unlocking
    }
    std::shared_ptr<int> load() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return ptr_;
    }
    static constexpr const char* how = "std::mutex";
private:
    mutable std::mutex mutex_;
    std::shared_ptr<int> ptr_;
};
#endif

int main()
{
    SharedConfig config(std::make_shared<int>(1));

    std::thread writer1([&] { for (int i = 0; i < 1000; ++i) config.store(std::make_shared<int>(i)); });
    std::thread writer2([&] { for (int i = 0; i < 1000; ++i) config.store(std::make_shared<int>(-i)); });
    writer1.join();
    writer2.join();

    std::shared_ptr<int> snapshot = config.load();
    std::cout << SharedConfig::how << ": done, last value is "
              << (*snapshot == 999 || *snapshot == -999 ? "from a final store" : "unexpected")
              << '\n';
}
