// cycle_leak.cpp - two objects that own each other through std::shared_ptr
// are never destroyed.
#include <iostream>
#include <memory>

struct Employee;

struct Team {
    ~Team() { std::cout << "  ~Team\n"; }
    std::shared_ptr<Employee> lead;
};

struct Employee {
    ~Employee() { std::cout << "  ~Employee\n"; }
    std::shared_ptr<Team> team;           // owning back-pointer: creates a cycle
};

void build_team()
{
    auto team = std::make_shared<Team>();
    auto lead = std::make_shared<Employee>();
    team->lead = lead;
    lead->team = team;
    std::cout << "  team.use_count() = " << team.use_count()
              << ", lead.use_count() = " << lead.use_count() << '\n';
}   // both locals are destroyed here; each object is still owned by the other

int main()
{
    build_team();
    std::cout << "  build_team() returned\n";
}
