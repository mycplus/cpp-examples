// producer_consumer.cpp - two producers and three consumers share a
// BoundedQueue with room for 8 items.
#include "bounded_queue.h"

#include <iostream>
#include <thread>
#include <vector>

int main()
{
    BoundedQueue<int> queue(8);
    constexpr int kPerProducer = 10'000;

    std::vector<std::thread> producers;
    for (int p = 0; p < 2; ++p)
        producers.emplace_back([&queue, p] {
            for (int i = 1; i <= kPerProducer; ++i)
                queue.push(p == 0 ? i : -i);       // producer 1 sends negatives
        });

    // Each consumer adds up what it receives in its own variables.
    std::vector<long> sums(3), counts(3);
    std::vector<std::thread> consumers;
    for (std::size_t c = 0; c < 3; ++c)
        consumers.emplace_back([&, c] {
            while (auto item = queue.pop()) {
                sums[c] += *item;
                ++counts[c];
            }
        });

    for (std::thread& t : producers)
        t.join();
    queue.close();                                 // no more items: let consumers finish
    for (std::thread& t : consumers)
        t.join();

    long total = 0, received = 0;
    for (std::size_t c = 0; c < 3; ++c) {
        total += sums[c];
        received += counts[c];
    }
    std::cout << "items received: " << received << '\n'
              << "sum (should be 0): " << total << '\n'
              << "push after close accepted: " << std::boolalpha << queue.push(1) << '\n';
}
