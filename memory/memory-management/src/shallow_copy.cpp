// shallow_copy.cpp - a class that frees its buffer in the destructor but lets
// the compiler generate the copy constructor. Both copies free the same block.
#include <cstring>
#include <iostream>

class Name {
public:
    explicit Name(const char* text) : data_(new char[std::strlen(text) + 1])
    {
        std::strcpy(data_, text);
    }
    ~Name() { delete[] data_; }                 // releases what the constructor allocated
    // No copy constructor or copy assignment: the generated ones copy the pointer.

    const char* c_str() const { return data_; }

private:
    char* data_;
};

int main()
{
    Name original("Ada Lovelace");
    {
        Name copy = original;                   // shallow copy: same data_ pointer
        std::cout << "copy:     " << copy.c_str() << '\n';
    }                                           // copy's destructor frees the shared block
    std::cout << "original: " << original.c_str() << '\n';
}                                               // original's destructor frees it again
