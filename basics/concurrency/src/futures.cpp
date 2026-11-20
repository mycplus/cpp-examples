// futures.cpp - getting results and exceptions back from other threads:
// std::async, std::promise and std::packaged_task.
#include <cstddef>
#include <future>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <vector>

long sum_range(const std::vector<int>& v, std::size_t from, std::size_t to)
{
    return std::accumulate(v.begin() + static_cast<std::ptrdiff_t>(from), v.begin() + static_cast<std::ptrdiff_t>(to), 0L);
}

int parse_positive(int value)
{
    if (value <= 0)
        throw std::invalid_argument("value must be positive");
    return value;
}

int main()
{
    std::vector<int> data(1000);
    std::iota(data.begin(), data.end(), 1);        // 1..1000

    // std::async: run a function and get its result through a future.
    // std::launch::async forces a new thread instead of possibly deferring.
    auto first_half = std::async(std::launch::async, sum_range, std::cref(data), std::size_t{0}, std::size_t{500});
    const long second_half = sum_range(data, 500, 1000);
    std::cout << "async sum: " << first_half.get() + second_half << '\n';

    // An exception thrown in the task is stored and rethrown by get().
    auto bad = std::async(std::launch::async, parse_positive, -5);
    try {
        bad.get();
    } catch (const std::invalid_argument& e) {
        std::cout << "exception from the other thread: " << e.what() << '\n';
    }

    // std::promise: a thread sets the value whenever it is ready.
    std::promise<std::string> promise;
    std::future<std::string> greeting = promise.get_future();
    std::thread sender([p = std::move(promise)]() mutable { p.set_value("hello from a promise"); });
    std::cout << greeting.get() << '\n';
    sender.join();

    // std::packaged_task: wrap a callable now, run it later on any thread.
    std::packaged_task<int(int)> task(parse_positive);
    std::future<int> result = task.get_future();
    std::thread runner(std::move(task), 42);
    std::cout << "packaged_task result: " << result.get() << '\n';
    runner.join();
}
