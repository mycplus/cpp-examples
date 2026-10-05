// cycle_fixed.cpp - the back-pointer becomes a std::weak_ptr, which observes
// the Team without keeping it alive.
#include <iostream>
#include <memory>

struct Employee;

struct Team {
    ~Team() { std::cout << "  ~Team\n"; }
    std::shared_ptr<Employee> lead;       // the team owns its lead
};

struct Employee {
    ~Employee() { std::cout << "  ~Employee\n"; }
    std::weak_ptr<Team> team;             // the lead only refers to the team

    void report() const
    {
        if (auto t = team.lock())         // a temporary owner, or null
            std::cout << "  team is alive, use_count while locked = " << t.use_count() << '\n';
        else
            std::cout << "  team no longer exists\n";
    }
};

int main()
{
    auto lead = std::make_shared<Employee>();
    {
        auto team = std::make_shared<Team>();
        team->lead = lead;
        lead->team = team;
        std::cout << "  team.use_count() = " << team.use_count()
                  << ", lead.use_count() = " << lead.use_count() << '\n';
        lead->report();
    }
    std::cout << "  left scope; expired() = " << std::boolalpha
              << lead->team.expired() << '\n';
    lead->report();
}
