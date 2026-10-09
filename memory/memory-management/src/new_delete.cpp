// new_delete.cpp - the four forms of new and delete, and what happens when
// an allocation cannot be satisfied.
#include <cstddef>
#include <iostream>
#include <new>
#include <string>
#include <utility>

// Storing the pointer in a volatile variable stops the optimizer from removing
// an allocation whose result is otherwise unused (C++ allows that removal).
char* volatile g_seen = nullptr;
void touch(char* p) { g_seen = p; }

struct Sensor {
    explicit Sensor(std::string n) : name(std::move(n)) { std::cout << "  construct " << name << '\n'; }
    ~Sensor() { std::cout << "  destroy   " << name << '\n'; }
    std::string name;
};

int main()
{
    std::cout << "single object:\n";
    Sensor* s = new Sensor("thermo");        // allocate, then construct
    delete s;                                // destroy, then deallocate

    std::cout << "array of objects:\n";
    Sensor* row = new Sensor[2]{Sensor("left"), Sensor("right")};
    delete[] row;                            // destroys both, last element first

    std::cout << "scalars:\n";
    int* uninitialized = new int[3];         // values are indeterminate: do not read them
    int* zeroed = new int[3]();              // () value-initializes: all zero
    std::cout << "  zeroed: " << zeroed[0] << ' ' << zeroed[1] << ' ' << zeroed[2] << '\n';
    delete[] uninitialized;
    delete[] zeroed;

    std::cout << "allocation failure:\n";
    std::size_t huge = std::size_t{1} << 46;   // 64 TiB
    try {
        char* p = new char[huge];
        touch(p);
        delete[] p;
        std::cout << "  allocated 64 TiB (unexpected)\n";
    } catch (const std::bad_alloc&) {
        std::cout << "  new threw std::bad_alloc\n";
    }

    char* q = new (std::nothrow) char[huge];  // returns nullptr instead of throwing
    touch(q);
    std::cout << "  new (std::nothrow) returned " << (q ? "a pointer" : "nullptr") << '\n';
    delete[] q;                              // deleting nullptr is a no-op
}
