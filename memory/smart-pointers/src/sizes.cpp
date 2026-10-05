// sizes.cpp - what each smart pointer costs in bytes on this platform.
#include <cstdio>
#include <memory>

struct FileCloser {                       // stateless deleter type
    void operator()(std::FILE* f) const noexcept { std::fclose(f); }
};

int main()
{
    auto lambda_closer = [](std::FILE* f) { std::fclose(f); };

    std::printf("%-46s %zu\n", "int*", sizeof(int*));
    std::printf("%-46s %zu\n", "std::unique_ptr<int>", sizeof(std::unique_ptr<int>));
    std::printf("%-46s %zu\n", "std::unique_ptr<FILE, FileCloser>",
                sizeof(std::unique_ptr<std::FILE, FileCloser>));
    std::printf("%-46s %zu\n", "std::unique_ptr<FILE, decltype(lambda)>",
                sizeof(std::unique_ptr<std::FILE, decltype(lambda_closer)>));
    std::printf("%-46s %zu\n", "std::unique_ptr<FILE, void (*)(FILE*)>",
                sizeof(std::unique_ptr<std::FILE, void (*)(std::FILE*)>));
    std::printf("%-46s %zu\n", "std::shared_ptr<int>", sizeof(std::shared_ptr<int>));
    std::printf("%-46s %zu\n", "std::weak_ptr<int>", sizeof(std::weak_ptr<int>));
}
