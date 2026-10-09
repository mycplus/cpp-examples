// throwing_destructor.cpp - since C++11 a destructor is noexcept unless it says
// otherwise, so an exception that leaves it calls std::terminate even when no
// other exception is in flight.
#include <cstring>
#include <iostream>
#include <stdexcept>

struct Flusher {                         // implicitly noexcept(true)
    ~Flusher() { throw std::runtime_error("flush failed"); }
};

struct LooseFlusher {                    // explicitly allows exceptions out
    ~LooseFlusher() noexcept(false) { throw std::runtime_error("flush failed"); }
};

int main(int argc, char** argv)
{
    const bool loose = argc > 1 && std::strcmp(argv[1], "loose") == 0;
    try {
        if (loose) {
            LooseFlusher f;
        } else {
            Flusher f;
        }
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << '\n';
    }
    std::cout << "end of main\n";
}
