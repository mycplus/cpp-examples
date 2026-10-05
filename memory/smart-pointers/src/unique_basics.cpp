// unique_basics.cpp - creating, moving, releasing and resetting std::unique_ptr.
#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct Widget {
    explicit Widget(int id) : id(id) { std::cout << "  Widget " << id << " created\n"; }
    ~Widget() { std::cout << "  Widget " << id << " destroyed\n"; }
    int id;
};

// A factory states in its signature that the caller becomes the owner.
std::unique_ptr<Widget> make_widget(int id)
{
    return std::make_unique<Widget>(id);
}

// A sink takes ownership by value; the Widget dies when this function returns.
void consume(std::unique_ptr<Widget> w)
{
    std::cout << "  consume() got Widget " << w->id << '\n';
}

int main()
{
    std::cout << "1. factory\n";
    auto a = make_widget(1);

    std::cout << "2. move into b\n";
    std::unique_ptr<Widget> b = std::move(a);
    std::cout << "  a is " << (a ? "non-null" : "null")
              << ", b owns Widget " << b->id << '\n';

    std::cout << "3. pass to a sink\n";
    consume(std::move(b));
    std::cout << "  back in main, b is " << (b ? "non-null" : "null") << '\n';

    std::cout << "4. reset replaces the owned object\n";
    auto c = std::make_unique<Widget>(2);
    c.reset(new Widget(3));      // Widget 3 is built, then Widget 2 is destroyed

    std::cout << "5. release gives up ownership without destroying\n";
    Widget* raw = c.release();
    std::cout << "  c is " << (c ? "non-null" : "null") << ", raw->id = " << raw->id << '\n';
    delete raw;                  // after release(), deleting is our job again

    std::cout << "6. array form\n";
    auto values = std::make_unique<int[]>(4);   // value-initialised: all zero
    values[2] = 7;
    for (int i = 0; i < 4; ++i)
        std::cout << "  values[" << i << "] = " << values[i] << '\n';

    std::cout << "7. end of main\n";
}
