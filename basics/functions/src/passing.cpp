// passing.cpp - count the copies each way of passing an argument makes.
#include <iostream>
#include <string>

struct Tracked {
    std::string name;
    static inline int copies = 0;

    explicit Tracked(std::string n) : name(std::move(n)) {}
    Tracked(const Tracked& other) : name(other.name) { ++copies; }
    Tracked& operator=(const Tracked& other) { name = other.name; ++copies; return *this; }
};

void by_value(Tracked t)            { t.name += "!"; }   // works on a copy
void by_const_ref(const Tracked& t) { (void)t.name.size(); }
void by_ref(Tracked& t)             { t.name += "!"; }   // changes the caller's object
void by_pointer(Tracked* t)         { if (t) t->name += "?"; }

Tracked make(const std::string& n)  { return Tracked{n}; }

int main() {
    Tracked doc{"report"};

    auto measure = [&](const char* label, auto call) {
        Tracked::copies = 0;
        call();
        std::cout << label << "copies: " << Tracked::copies << "  name: " << doc.name << '\n';
    };

    measure("by value:           ", [&] { by_value(doc); });
    measure("by const reference: ", [&] { by_const_ref(doc); });
    measure("by reference:       ", [&] { by_ref(doc); });
    measure("by pointer:         ", [&] { by_pointer(&doc); });
    measure("null pointer:       ", [&] { by_pointer(nullptr); });

    Tracked::copies = 0;
    Tracked fresh = make("draft");
    std::cout << "returned by value:  copies: " << Tracked::copies << "  name: " << fresh.name << '\n';
}
