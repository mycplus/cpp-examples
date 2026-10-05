// shared_basics.cpp - how std::shared_ptr's use_count moves with copies,
// moves, scopes and reset().
#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct Texture {
    explicit Texture(std::string n) : name(std::move(n)) { std::cout << "  load " << name << '\n'; }
    ~Texture() { std::cout << "  free " << name << '\n'; }
    std::string name;
};

int main()
{
    auto first = std::make_shared<Texture>("grass.png");
    std::cout << "after make_shared:  use_count = " << first.use_count() << '\n';

    {
        std::shared_ptr<Texture> second = first;            // copy: +1
        std::cout << "after copy:         use_count = " << first.use_count() << '\n';

        std::shared_ptr<Texture> third = std::move(second); // move: no change
        std::cout << "after move:         use_count = " << first.use_count()
                  << " (second is " << (second ? "non-null" : "null") << ")\n";
    }
    std::cout << "after inner scope:  use_count = " << first.use_count() << '\n';

    first.reset();                                          // last owner: object freed
    std::cout << "after reset:        first is " << (first ? "non-null" : "null") << '\n';
}
