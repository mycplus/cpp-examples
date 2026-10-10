// inheriting_ctors.cpp - `using Base::Base;` gives a derived class the base
// class's constructors (C++11).
#include <iostream>
#include <string>
#include <utility>

class Connection {
public:
    explicit Connection(std::string host, int port = 443)
        : host_(std::move(host)), port_(port) {}
    std::string address() const { return host_ + ":" + std::to_string(port_); }

private:
    std::string host_;
    int port_;
};

class LoggedConnection : public Connection {
public:
    using Connection::Connection;      // both constructor forms, no boilerplate
    void log() const { std::cout << "connected to " << address() << '\n'; }
};

int main() {
    LoggedConnection a("example.com");
    LoggedConnection b("localhost", 8080);
    a.log();
    b.log();
}
