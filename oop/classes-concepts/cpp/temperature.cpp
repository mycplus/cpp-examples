// temperature.cpp - one class in C++: private state, a validating constructor,
// a computed value, a static factory and a static counter.
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Temperature {
public:
    explicit Temperature(double celsius) {
        if (celsius < -273.15)
            throw std::invalid_argument("below absolute zero: " + format(celsius) + " C");
        celsius_ = celsius;
        ++created_;                                   // counts objects, not calls
    }

    static Temperature from_fahrenheit(double f) { return Temperature((f - 32) * 5 / 9); }
    static int created() { return created_; }

    double celsius() const { return celsius_; }
    double fahrenheit() const { return celsius_ * 9 / 5 + 32; }

    std::string to_string() const {
        return format(celsius()) + " C = " + format(fahrenheit()) + " F";
    }

private:
    static std::string format(double v) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(1) << v;
        return out.str();
    }

    double celsius_ = 0;                              // one per object
    static inline int created_ = 0;                   // one per class (C++17)
};

int main() {
    Temperature room(21.5);
    Temperature boiling(100);
    Temperature cold = Temperature::from_fahrenheit(-40);

    for (const Temperature* t : {&room, &boiling, &cold})
        std::cout << t->to_string() << '\n';
    std::cout << "objects created: " << Temperature::created() << '\n';

    try {
        Temperature impossible(-300);
    } catch (const std::invalid_argument& e) {
        std::cout << "error: " << e.what() << '\n';
    }
    std::cout << "objects created: " << Temperature::created() << '\n';
}
