// overlapped_server.cpp - one thread serves several clients at once with
// overlapped (asynchronous) I/O. Every pipe instance has an OVERLAPPED
// structure with its own event, and WaitForMultipleObjects reports which
// instance finished an operation.
//
// usage: overlapped_server [name] [clients]    defaults: demo 3
#include "pipe_common.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr DWORD kInstances = 4;    // WaitForMultipleObjects allows up to 64

enum class State { Connecting, Reading, Writing };

struct Instance {
    UniqueHandle pipe;
    UniqueHandle event;
    OVERLAPPED overlapped{};
    State state = State::Connecting;
    char buffer[512];
    std::string request;
    std::string reply;
    int client = 0;              // 0 while no client is connected
    bool connect_pending = false;  // false: the client arrived before ConnectNamedPipe
};

// Starts an asynchronous connect. Returns false on a real error.
bool start_connect(Instance& in)
{
    in.state = State::Connecting;
    if (ConnectNamedPipe(in.pipe.get(), &in.overlapped))
        return false;    // overlapped ConnectNamedPipe should never return TRUE
    switch (GetLastError()) {
    case ERROR_IO_PENDING:
        in.connect_pending = true;     // the event is signaled when a client arrives
        return true;
    case ERROR_PIPE_CONNECTED:
        in.connect_pending = false;    // a client arrived already: no I/O to wait for,
        SetEvent(in.event.get());      // so signal the event by hand
        return true;
    case ERROR_NO_DATA:
        DisconnectNamedPipe(in.pipe.get());    // a client came and went: listen again
        return start_connect(in);
    default:
        return false;
    }
}

// Starts an asynchronous read or write. ERROR_IO_PENDING is the normal case;
// a call that finished at once signals the event too, so both are handled in
// the same place: the main loop.
bool start_io(Instance& in)
{
    BOOL ok;
    if (in.state == State::Reading)
        ok = ReadFile(in.pipe.get(), in.buffer, static_cast<DWORD>(sizeof in.buffer), nullptr, &in.overlapped);
    else
        ok = WriteFile(in.pipe.get(), in.reply.data(), static_cast<DWORD>(in.reply.size()),
                       nullptr, &in.overlapped);
    return ok || GetLastError() == ERROR_IO_PENDING || GetLastError() == ERROR_MORE_DATA;
}

}    // namespace

int main(int argc, char** argv)
{
    const std::string name = argc > 1 ? argv[1] : "demo";
    const int clients = argc > 2 ? std::atoi(argv[2]) : 3;
    const std::wstring path = pipe_path(name);

    std::vector<Instance> instances(kInstances);
    std::vector<HANDLE> events;
    for (DWORD i = 0; i < kInstances; ++i) {
        Instance& in = instances[i];
        in.event = UniqueHandle(CreateEventW(nullptr, TRUE, FALSE, nullptr));    // manual reset
        in.overlapped.hEvent = in.event.get();
        in.pipe = UniqueHandle(CreateNamedPipeW(
            path.c_str(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | (i == 0 ? FILE_FLAG_FIRST_PIPE_INSTANCE : 0),
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
            kInstances, 4096, 4096, 0, nullptr));
        if (!in.event.valid() || !in.pipe.valid() || !start_connect(in)) {
            std::cout << "server: setting up instance " << i << " failed: "
                      << error_text(GetLastError()) << '\n';
            return 1;
        }
        events.push_back(in.event.get());
    }
    std::cout << "server: listening on pipe mycplus-" << name << " with " << kInstances
              << " instances" << std::endl;

    int connected = 0;
    int finished = 0;
    while (finished < clients) {
        const DWORD signaled = WaitForMultipleObjects(static_cast<DWORD>(events.size()),
                                                      events.data(), FALSE, INFINITE);
        const DWORD i = signaled - WAIT_OBJECT_0;
        if (i >= kInstances) {
            std::cout << "server: WaitForMultipleObjects failed: " << error_text(GetLastError()) << '\n';
            return 1;
        }
        Instance& in = instances[i];

        DWORD bytes = 0;
        const bool no_io = in.state == State::Connecting && !in.connect_pending;
        const BOOL ok = no_io || GetOverlappedResult(in.pipe.get(), &in.overlapped, &bytes, FALSE);
        const DWORD error = ok ? static_cast<DWORD>(ERROR_SUCCESS) : GetLastError();
        ResetEvent(in.event.get());

        bool next_ok = true;
        if (in.state == State::Connecting) {
            if (!ok) {
                std::cout << "server: connect failed: " << error_text(error) << '\n';
                return 1;
            }
            in.client = ++connected;
            std::cout << "server: client " << in.client << " connected on instance " << i << '\n';
            in.request.clear();
            in.state = State::Reading;
            next_ok = start_io(in);
        } else if (in.state == State::Reading && (ok || error == ERROR_MORE_DATA)) {
            in.request.append(in.buffer, bytes);
            if (error == ERROR_MORE_DATA) {
                next_ok = start_io(in);              // read the rest of the message
            } else {
                std::cout << "server: client " << in.client << " sent \"" << in.request << "\"\n";
                in.reply = make_reply(in.request);
                in.state = State::Writing;
                next_ok = start_io(in);
            }
        } else if (in.state == State::Writing && ok && bytes == in.reply.size()) {
            in.request.clear();
            in.state = State::Reading;
            next_ok = start_io(in);
        } else {
            next_ok = false;    // the client closed its end, or the transfer failed
            if (error != ERROR_BROKEN_PIPE)
                std::cout << "server: client " << in.client << " failed: " << error_text(error) << '\n';
        }

        if (!next_ok) {
            if (in.state != State::Connecting && in.client != 0) {
                std::cout << "server: client " << in.client << " disconnected\n";
                ++finished;
            }
            in.client = 0;
            DisconnectNamedPipe(in.pipe.get());    // recycle the instance for a new client
            if (finished < clients && !start_connect(in)) {
                std::cout << "server: reconnect failed: " << error_text(GetLastError()) << '\n';
                return 1;
            }
        }
    }
    // Instances still waiting for a client have a ConnectNamedPipe pending that
    // uses their OVERLAPPED and event. Cancel it and wait for the cancellation
    // before those objects are destroyed.
    for (Instance& in : instances) {
        DWORD ignored = 0;
        if (CancelIoEx(in.pipe.get(), &in.overlapped))
            GetOverlappedResult(in.pipe.get(), &in.overlapped, &ignored, TRUE);
    }
    std::cout << "server: clients served: " << finished << std::endl;
    return 0;
}
