// threads.cpp - which std::shared_ptr operations are safe across threads.
//   threads          each thread copies the shared_ptr into its own local (safe)
//   threads assign   both threads assign to the same shared_ptr object (a data race)
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>

int main(int argc, char** argv)
{
    auto config = std::make_shared<int>(1);

    if (argc == 2 && std::string_view(argv[1]) == "assign") {
        std::thread t1([&] { for (int i = 0; i < 1000; ++i) config = std::make_shared<int>(i); });
        std::thread t2([&] { for (int i = 0; i < 1000; ++i) config = std::make_shared<int>(-i); });
        t1.join();
        t2.join();
        std::cout << "shared assignment: done\n";
        return 0;
    }

    auto reader = [config] {                  // each closure holds its own copy
        for (int i = 0; i < 100000; ++i) {
            std::shared_ptr<int> local = config;
            (void)local;
        }
    };
    std::thread t1(reader), t2(reader);
    t1.join();
    t2.join();
    std::cout << "copies: done, use_count = " << config.use_count() << '\n';
}
