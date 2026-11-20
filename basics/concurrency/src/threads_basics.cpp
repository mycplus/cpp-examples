// threads_basics.cpp - starting threads, passing arguments, joining, and
// std::jthread, which joins by itself.
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void fill(std::vector<int>& out, int from, int count)
{
    for (int i = 0; i < count; ++i)
        out.push_back(from + i);
}

int main()
{
    // 1. A thread runs a function; join() waits for it to finish.
    std::vector<int> numbers;
    std::thread worker(fill, std::ref(numbers), 10, 5);   // std::ref: pass by reference
    worker.join();
    std::cout << "worker filled " << numbers.size() << " numbers, first " << numbers.front() << '\n';

    // 2. Arguments are copied into the thread unless wrapped in std::ref.
    std::string label = "unchanged";
    std::thread copier([](std::string s) { s += " (changed in thread)"; }, label);
    copier.join();
    std::cout << "label after thread: " << label << '\n';

    // 3. Each thread writes only its own slot, so no locking is needed.
    std::vector<long> partial(4);
    std::vector<std::thread> pool;
    for (int t = 0; t < 4; ++t)
        pool.emplace_back([&partial, t] {
            for (long i = t * 250; i < (t + 1) * 250; ++i)
                partial[static_cast<std::size_t>(t)] += i;
        });
    for (std::thread& th : pool)
        th.join();
    long total = 0;
    for (long p : partial)
        total += p;
    std::cout << "sum of 0..999 from 4 threads: " << total << '\n';

    // 4. A std::thread that is still joinable when destroyed calls std::terminate.
    //    std::jthread (C++20) joins in its destructor instead. See cpp20_sync.cpp.
    std::cout << "hardware threads reported: " << (std::thread::hardware_concurrency() > 0 ? "at least 1" : "unknown") << '\n';
}
