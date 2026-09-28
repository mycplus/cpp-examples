// recursive_destructor.cpp - DO NOT COPY. A linked stack that relies on the
// default destructor: each unique_ptr destroys the next node from inside its
// own destructor, so freeing N nodes nests N calls deep.
#include <cstdio>
#include <cstdlib>
#include <memory>

struct Node {
    int value;
    std::unique_ptr<Node> next;
};

int main(int argc, char** argv)
{
    const int n = argc > 1 ? std::atoi(argv[1]) : 1'000'000;
    std::unique_ptr<Node> head;
    for (int i = 0; i < n; ++i)
        head = std::make_unique<Node>(Node{i, std::move(head)});
    std::printf("built %d nodes; destroying\n", n);
    std::fflush(stdout);
    head.reset();                 // recursion one frame per node
    std::puts("destroyed");
    return EXIT_SUCCESS;
}
