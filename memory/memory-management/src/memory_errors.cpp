// memory_errors.cpp - eight memory errors that compile, one per command-line mode.
// Run it plainly to see what the program does, then build it with
// -fsanitize=address, or run it under Valgrind, to see what the tools report.
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

[[gnu::noinline]] int* make_scores(std::size_t n)
{
    int* p = new int[n];
    for (std::size_t i = 0; i < n; ++i)
        p[i] = static_cast<int>(i);
    return p;
}

void leak()
{
    int* scores = make_scores(100);
    std::printf("first score %d\n", scores[0]);
}                                              // no delete[]: 400 bytes lost

void use_after_free()
{
    int* scores = make_scores(100);
    delete[] scores;
    std::printf("read after delete[]: %d\n", scores[10]);
}

void double_free()
{
    int* scores = make_scores(100);
    delete[] scores;
    delete[] scores;                           // second release of the same block
    std::printf("deleted twice\n");
}

void mismatched_delete()
{
    std::string* names = new std::string[3]{"ada", "grace", "linus"};
    std::printf("first name %s\n", names[0].c_str());
    delete names;                              // allocated with new[], needs delete[]
}

void heap_overflow()
{
    std::size_t n = 10;
    int* scores = make_scores(n);
    for (std::size_t i = 0; i <= n; ++i)       // <= writes one element past the end
        scores[i] = 0;
    std::printf("cleared %zu scores\n", n);
    delete[] scores;
}

const std::string& longest(const std::vector<std::string>& words)
{
    std::string best;                          // local: destroyed when the function returns
    for (const auto& w : words)
        if (w.size() > best.size())
            best = w;
    return best;                               // returns a reference to a dead object
}

void return_local()
{
    const std::vector<std::string> words{"heap", "stack", "allocator"};
    const std::string& best = longest(words);
    std::printf("longest word: %s\n", best.c_str());
}

void invalidated_pointer()
{
    std::vector<int> values{1, 2, 3};
    int* first = &values[0];                   // points into the vector's buffer
    values.push_back(4);                       // may reallocate and free that buffer
    std::printf("first value: %d\n", *first);
}

void delete_non_heap()
{
    int local = 42;
    int* p = &local;
    std::printf("deleting a stack address\n");
    delete p;                                  // p was never returned by new
}

struct Mode { const char* name; void (*run)(); };

constexpr Mode modes[] = {
    {"leak", leak},
    {"use-after-free", use_after_free},
    {"double-free", double_free},
    {"mismatched-delete", mismatched_delete},
    {"heap-overflow", heap_overflow},
    {"return-local", return_local},
    {"invalidated-pointer", invalidated_pointer},
    {"delete-non-heap", delete_non_heap},
};

} // namespace

int main(int argc, char** argv)
{
    if (argc == 2)
        for (const Mode& m : modes)
            if (std::strcmp(argv[1], m.name) == 0) {
                m.run();
                return 0;
            }
    std::fprintf(stderr, "usage: memory_errors <mode>\nmodes:");
    for (const Mode& m : modes)
        std::fprintf(stderr, " %s", m.name);
    std::fprintf(stderr, "\n");
    return 2;
}
