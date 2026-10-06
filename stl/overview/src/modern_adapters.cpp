// modern_adapters.cpp - C++98 function adapters and their replacements.
// Each block reproduces a classic example with C++17 tools.
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

static bool is_even(int x) { return x % 2 == 0; }

class Angle {
public:
    explicit Angle(int deg) : degrees_(deg) {}
    int mul(int times) { return degrees_ *= times; }
private:
    int degrees_;
};

struct Shape {
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};
struct Circle : Shape { void draw() const override { std::cout << "Circle::draw "; } };
struct Square : Shape { void draw() const override { std::cout << "Square::draw "; } };

int main()
{
    const std::vector<int> d{123, 94, 10, 314, 315};

    // not1(ptr_fun(is_even))  ->  std::not_fn(is_even)
    std::cout << "not_fn:  ";
    std::transform(d.begin(), d.end(), std::ostream_iterator<bool>(std::cout, " "),
                   std::not_fn(is_even));

    // bind2nd(ptr_fun<double, double, double>(pow), 2.0)  ->  a lambda
    const std::vector<double> x{1.23, 91.370, 56.661, 23.230, 19.959, 1.0, 3.14159};
    std::cout << "\nsquares: ";
    std::transform(x.begin(), x.end(), std::ostream_iterator<double>(std::cout, " "),
                   [](double v) { return std::pow(v, 2.0); });

    // mem_fun_ref(&Angle::mul) with two ranges  ->  std::mem_fn
    std::vector<Angle> angles;
    for (int deg = 0; deg < 50; deg += 10) angles.emplace_back(deg);
    const std::vector<int> times{1, 2, 3, 4, 5};
    std::cout << "\nmem_fn:  ";
    std::transform(angles.begin(), angles.end(), times.begin(),
                   std::ostream_iterator<int>(std::cout, " "), std::mem_fn(&Angle::mul));

    // mem_fun(&Shape::draw) over raw pointers, then a manual purge()
    //   ->  std::mem_fn over unique_ptr; no cleanup code
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>());
    shapes.push_back(std::make_unique<Square>());
    std::cout << "\nshapes:  ";
    std::for_each(shapes.begin(), shapes.end(), std::mem_fn(&Shape::draw));

    // bind1st(equal_to<int>(), 20) and not1(...)  ->  std::bind, or a lambda
    auto equals_20 = std::bind(std::equal_to<int>{}, 20, std::placeholders::_1);
    const std::vector<int> e{10, 20, 30, 20};
    std::cout << "\nbind:    " << std::count_if(e.begin(), e.end(), equals_20)
              << " equal to 20, "
              << std::count_if(e.begin(), e.end(), [](int v) { return v != 20; })
              << " not equal\n";
}
