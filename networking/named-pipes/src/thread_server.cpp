// thread_server.cpp - a multithreaded named pipe server. The main thread
// waits for clients; each connected client gets its own std::thread.
//
// usage: thread_server [name] [clients]    defaults: demo 3
#include "pipe_common.h"

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

static std::mutex print_mutex;    // one line at a time on std::cout

static void say(const std::string& line)
{
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << line << std::endl;
}

// Runs on its own thread and owns its pipe instance until the client leaves.
static void serve(UniqueHandle pipe, int id)
{
    std::string request;
    DWORD error = 0;
    while (read_message(pipe.get(), request, &error)) {
        say("server: client " + std::to_string(id) + " sent \"" + request + "\"");
        if (!write_message(pipe.get(), make_reply(request), &error))
            break;
    }
    if (error != ERROR_BROKEN_PIPE)
        say("server: client " + std::to_string(id) + " failed: " + error_text(error));
    FlushFileBuffers(pipe.get());
    DisconnectNamedPipe(pipe.get());
    say("server: client " + std::to_string(id) + " disconnected");
}   // the UniqueHandle closes this instance here

int main(int argc, char** argv)
{
    const std::string name = argc > 1 ? argv[1] : "demo";
    const int clients = argc > 2 ? std::atoi(argv[2]) : 3;
    const std::wstring path = pipe_path(name);

    std::vector<std::thread> workers;
    int result = 0;
    for (int id = 1; id <= clients; ++id) {
        // A new instance for every client. Only the first may claim the name.
        const DWORD open_mode = PIPE_ACCESS_DUPLEX | (id == 1 ? FILE_FLAG_FIRST_PIPE_INSTANCE : 0);
        UniqueHandle pipe(CreateNamedPipeW(
            path.c_str(), open_mode,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
            PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, nullptr));
        if (!pipe.valid()) {
            say("server: CreateNamedPipe failed: " + error_text(GetLastError()));
            result = 1;
            break;
        }
        if (id == 1)
            say("server: listening on pipe mycplus-" + name);

        if (!ConnectNamedPipe(pipe.get(), nullptr) && GetLastError() != ERROR_PIPE_CONNECTED) {
            say("server: ConnectNamedPipe failed: " + error_text(GetLastError()));
            result = 1;
            break;
        }
        say("server: client " + std::to_string(id) + " connected");
        workers.emplace_back(serve, std::move(pipe), id);
    }

    for (std::thread& worker : workers)
        worker.join();    // wait for every client to finish before exiting
    return result;
}
