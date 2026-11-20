// cpp20_sync.cpp - the C++20 additions: std::jthread with a stop token,
// std::latch, std::barrier and std::counting_semaphore.
#include <algorithm>
#include <atomic>
#include <barrier>
#include <chrono>
#include <iostream>
#include <latch>
#include <semaphore>
#include <thread>
#include <vector>

int main()
{
    // 1. jthread: request_stop() asks the thread to finish; the destructor joins.
    std::atomic<int> ticks{0};
    {
        std::jthread ticker([&ticks](std::stop_token stop) {
            while (!stop.stop_requested()) {
                ++ticks;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        while (ticks < 5)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ticker.request_stop();
    }   // joined here, no explicit join() needed
    std::cout << "jthread stopped after at least 5 ticks: " << std::boolalpha << (ticks >= 5) << '\n';

    // 2. latch: a one-shot countdown. main waits until all 3 workers are ready.
    std::latch ready(3);
    std::vector<std::jthread> workers;
    for (int i = 0; i < 3; ++i)
        workers.emplace_back([&ready] { ready.count_down(); });
    ready.wait();
    std::cout << "latch: all 3 workers ready\n";
    workers.clear();   // jthreads join as they are destroyed

    // 3. barrier: threads meet at the end of each phase; the completion
    //    function runs once per phase, before anyone continues.
    int phases_done = 0;
    std::barrier sync(3, [&phases_done]() noexcept { ++phases_done; });
    for (int i = 0; i < 3; ++i)
        workers.emplace_back([&sync] {
            for (int phase = 0; phase < 4; ++phase)
                sync.arrive_and_wait();
        });
    workers.clear();
    std::cout << "barrier: " << phases_done << " phases completed\n";

    // 4. counting_semaphore: at most 2 of the 6 threads inside at once.
    std::counting_semaphore<2> slots(2);
    std::atomic<int> inside{0}, most{0};
    for (int i = 0; i < 6; ++i)
        workers.emplace_back([&] {
            slots.acquire();
            const int now = ++inside;
            int seen = most.load();
            while (now > seen && !most.compare_exchange_weak(seen, now)) {
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            --inside;
            slots.release();
        });
    workers.clear();
    std::cout << "semaphore: at most " << most.load() << " threads inside at once\n";
}
