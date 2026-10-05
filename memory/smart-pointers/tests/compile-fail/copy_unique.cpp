// copy_unique.cpp - must NOT compile: std::unique_ptr has no copy constructor.
#include <memory>

int main()
{
    auto a = std::make_unique<int>(42);
    auto b = a;
    return *b;
}
