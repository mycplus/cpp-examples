// test_deep_destroy.cpp - destroys a 20,000-node chain on a thread whose
// stack is 64 KiB. A destructor that recursed once per node would need
// far more than that; clear() rotates instead of recursing. POSIX only.
// Build: g++ -std=c++17 -Wall -Wextra -pedantic -pthread -I../include test_deep_destroy.cpp -o test_deep_destroy
#include "binary_search_tree.hpp"

#include <pthread.h>

#include <cstdio>

namespace {

void* build_and_destroy(void* result)
{
    {
        BinarySearchTree<int> chain;
        for (int k = 0; k < 20000; ++k)    // sorted keys: a right-leaning chain
            chain.insert(k);
        *static_cast<long*>(result) = chain.height();
    }                                      // destroyed here, on the small stack
    return nullptr;
}

}  // namespace

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 64 * 1024);
    pthread_t thread;
    long height = 0;
    if (pthread_create(&thread, &attr, build_and_destroy, &height) != 0) {
        std::puts("could not start the thread");
        return 1;
    }
    pthread_join(thread, nullptr);
    pthread_attr_destroy(&attr);
    std::printf("chain of 20000: height %ld, destroyed on a 64 KiB stack\n", height);
    return height == 19999 ? 0 : 1;
}
