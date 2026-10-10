// name_clash.cpp - two bases with a member of the same name.
#include <iostream>

struct Engine {
    int power() const { return 150; }      // kilowatts
    void start() const { std::cout << "engine started\n"; }
};

struct Battery {
    int power() const { return 60; }       // kilowatt-hours
    void charge() const { std::cout << "battery charging\n"; }
};

struct Hybrid : Engine, Battery {
    // Pick one meaning for the unqualified name,
    using Engine::power;
    // and give the other a name of its own.
    int capacity() const { return Battery::power(); }
};

int main() {
    Hybrid car;
    car.start();
    car.charge();
    std::cout << "power:    " << car.power() << " kW\n";
    std::cout << "capacity: " << car.capacity() << " kWh\n";
    std::cout << "qualified: " << car.Engine::power() << " and " << car.Battery::power() << '\n';
}
