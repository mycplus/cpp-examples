// greeting.cpp - abstraction: greeting() depends on what a clock does
// (report the hour), not on how any particular clock finds it out.
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

class Clock {
public:
    virtual ~Clock() = default;
    virtual int hour() const = 0;  // 0 to 23; every concrete clock must say how
};

class SystemClock : public Clock {
public:
    int hour() const override {
        std::time_t now = std::time(nullptr);
        return std::localtime(&now)->tm_hour;
    }
};

class FixedClock : public Clock {
public:
    explicit FixedClock(int hour) : hour_(hour) {}
    int hour() const override { return hour_; }

private:
    int hour_;
};

std::string greeting(const Clock& clock) {
    int h = clock.hour();
    if (h < 12) return "Good morning";
    if (h < 18) return "Good afternoon";
    return "Good evening";
}

int main() {
    for (int h : {8, 14, 21}) {
        FixedClock clock(h);
        std::cout << std::setw(2) << std::setfill('0') << h << ":00 -> " << greeting(clock) << '\n';
    }

    SystemClock now;
    std::string g = greeting(now);
    bool known = g == "Good morning" || g == "Good afternoon" || g == "Good evening";
    std::cout << "system clock: hour in 0..23 = " << std::boolalpha
              << (now.hour() >= 0 && now.hour() <= 23) << ", greeting known = " << known << '\n';

    // Clock c;  // does not compile: Clock is abstract
}
