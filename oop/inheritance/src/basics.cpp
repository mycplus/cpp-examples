// basics.cpp - a derived class reuses its base class's members and adds its own.
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Employee {
public:
    Employee(std::string name, double salary) : name_(std::move(name)), salary_(salary) {}

    const std::string& name() const { return name_; }
    double salary() const { return salary_; }
    void raise(double percent) { salary_ += salary_ * percent / 100.0; }

private:
    std::string name_;
    double salary_;
};

// A Manager is an Employee, plus a list of direct reports.
class Manager : public Employee {
public:
    Manager(std::string name, double salary) : Employee(std::move(name), salary) {}

    void add_report(const Employee& e) { reports_.push_back(e.name()); }
    std::size_t team_size() const { return reports_.size(); }

private:
    std::vector<std::string> reports_;
};

void print_badge(const Employee& e) {          // accepts a Manager too
    std::cout << "badge: " << e.name() << '\n';
}

int main() {
    Employee dev("Ada", 5000);
    Manager lead("Grace", 7000);

    lead.add_report(dev);
    lead.raise(10);                            // inherited from Employee

    std::cout << lead.name() << " earns " << lead.salary()
              << " and manages " << lead.team_size() << " person\n";
    print_badge(dev);
    print_badge(lead);
}
