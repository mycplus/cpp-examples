// notifications.cpp - inheritance in C++: a base class with shared behavior,
// two derived classes that override one step of it.
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Notification {
public:
    explicit Notification(std::string recipient) : recipient_(std::move(recipient)) {}
    virtual ~Notification() = default;

    // Shared by every kind of notification.
    void send(const std::string& message) const {
        std::cout << "to " << recipient_ << ": " << format(message) << '\n';
    }

protected:
    // The step each kind can change.
    virtual std::string format(const std::string& message) const { return message; }

private:
    std::string recipient_;
};

class Email : public Notification {
public:
    using Notification::Notification;
protected:
    std::string format(const std::string& message) const override {
        return "[email] " + Notification::format(message);    // extend the parent's version
    }
};

class Sms : public Notification {
public:
    using Notification::Notification;
protected:
    std::string format(const std::string& message) const override {
        return "[sms] " + message.substr(0, 19);               // replace it
    }
};

int main() {
    std::vector<std::unique_ptr<Notification>> outbox;
    outbox.push_back(std::make_unique<Notification>("ops-team"));
    outbox.push_back(std::make_unique<Email>("ada@example.com"));
    outbox.push_back(std::make_unique<Sms>("+1-555-0100"));

    for (const auto& n : outbox)
        n->send("Disk usage on db-01 is above 90 percent");
}
