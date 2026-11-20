// counter.cpp - four threads increment one counter 100,000 times each.
//   counter mutex    a std::mutex protects the counter       (always 400000)
//   counter atomic   the counter is a std::atomic<long>       (always 400000)
//   counter racy     no protection: a data race, undefined behavior.
//                    Build with -fsanitize=thread to see it reported.
#include <atomic>
#include <iostream>
#include <mutex>
#include <string_view>
#include <thread>
#include <vector>

constexpr int kThreads = 4;
constexpr int kIncrements = 100'000;

template <class Increment>
void run(Increment increment)
{
    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t)
        threads.emplace_back([&] {
            for (int i = 0; i < kIncrements; ++i)
                increment();
        });
    for (std::thread& t : threads)
        t.join();
}

int main(int argc, char** argv)
{
    const std::string_view mode = argc > 1 ? argv[1] : "mutex";

    if (mode == "mutex") {
        long count = 0;
        std::mutex m;
        run([&] {
            std::lock_guard<std::mutex> lock(m);   // unlocked when lock goes out of scope
            ++count;
        });
        std::cout << "mutex:  " << count << '\n';
    } else if (mode == "atomic") {
        std::atomic<long> count{0};
        run([&] { count.fetch_add(1, std::memory_order_relaxed); });
        std::cout << "atomic: " << count.load() << '\n';
    } else if (mode == "racy") {
        long count = 0;
        run([&] { ++count; });                     // two threads may read the same old value
        std::cout << "racy:   " << count << " (expected 400000)\n";
    } else {
        std::cout << "usage: counter mutex|atomic|racy\n";
        return 2;
    }
}
