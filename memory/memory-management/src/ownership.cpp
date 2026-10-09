// ownership.cpp - two correct versions of a class that owns a character buffer:
// one that manages the memory itself (rule of five) and one that lets a
// standard type do it (rule of zero).
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

// Rule of five: every special member function is written by hand.
class Buffer {
public:
    explicit Buffer(const char* text)
        : size_(std::strlen(text)), data_(new char[size_ + 1])
    {
        std::memcpy(data_, text, size_ + 1);
    }
    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other)                         // deep copy
        : size_(other.size_), data_(new char[other.size_ + 1])
    {
        std::memcpy(data_, other.data_, size_ + 1);
    }
    Buffer(Buffer&& other) noexcept                     // steal, leave the source empty
        : size_(std::exchange(other.size_, 0)), data_(std::exchange(other.data_, nullptr)) {}

    Buffer& operator=(Buffer other) noexcept            // copy-and-swap: handles both
    {                                                   // copy and move assignment,
        std::swap(size_, other.size_);                  // and self-assignment
        std::swap(data_, other.data_);
        return *this;
    }

    void set_first(char c) { if (size_ > 0) data_[0] = c; }
    const char* c_str() const { return data_ ? data_ : ""; }

private:
    std::size_t size_;      // declared before data_, which is initialized from it
    char* data_;
};

// Rule of zero: std::string already copies, moves and frees correctly.
struct Name {
    std::string text;
};

int main()
{
    Buffer a("Ada Lovelace");
    Buffer b = a;                       // copy constructor
    b.set_first('I');
    Buffer c = std::move(b);            // move constructor
    Buffer& same = a;
    a = same;                           // self-assignment is safe
    std::cout << "Buffer: a=\"" << a.c_str() << "\" b=\"" << b.c_str()
              << "\" c=\"" << c.c_str() << "\"\n";

    Name x{"Grace Hopper"};
    Name y = x;                         // generated copy: a separate string
    y.text[0] = 'T';
    Name z = std::move(y);              // generated move
    std::cout << "Name:   x=\"" << x.text << "\" z=\"" << z.text << "\"\n";
}
