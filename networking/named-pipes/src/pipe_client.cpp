// pipe_client.cpp - connects to a named pipe server, sends each argument as
// one message, and prints the reply.
//
// usage: pipe_client [name] [message...]    defaults: demo "hello"
#include "pipe_common.h"

#include <iostream>
#include <string>
#include <vector>

// Opens the pipe, waiting for a free instance. Gives up after about 5 seconds.
static UniqueHandle connect(const std::wstring& path)
{
    for (int attempt = 0; attempt < 50; ++attempt) {
        UniqueHandle pipe(CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                      OPEN_EXISTING, 0, nullptr));
        if (pipe.valid())
            return pipe;

        const DWORD error = GetLastError();
        if (error == ERROR_PIPE_BUSY)
            WaitNamedPipeW(path.c_str(), 100);    // every instance is busy: wait for one
        else if (error == ERROR_FILE_NOT_FOUND)
            Sleep(100);                           // the server has not created the pipe yet
        else {
            std::cout << "client: CreateFile failed: " << error_text(error) << '\n';
            return UniqueHandle();
        }
    }
    std::cout << "client: no server after 5 seconds\n";
    return UniqueHandle();
}

int main(int argc, char** argv)
{
    const std::string name = argc > 1 ? argv[1] : "demo";
    std::vector<std::string> messages(argv + (argc > 2 ? 2 : argc), argv + argc);
    if (messages.empty())
        messages.push_back("hello");

    UniqueHandle pipe = connect(pipe_path(name));
    if (!pipe.valid())
        return 1;

    // A client handle starts in byte mode; switch it to message mode so each
    // ReadFile returns one whole reply.
    DWORD mode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(pipe.get(), &mode, nullptr, nullptr)) {
        std::cout << "client: SetNamedPipeHandleState failed: " << error_text(GetLastError()) << '\n';
        return 1;
    }

    for (const std::string& message : messages) {
        DWORD error = 0;
        std::string reply;
        if (!write_message(pipe.get(), message, &error) || !read_message(pipe.get(), reply, &error)) {
            std::cout << "client: transfer failed: " << error_text(error) << '\n';
            return 1;
        }
        std::cout << "client: sent \"" << message << "\", got \"" << reply << "\"\n";
    }
    return 0;    // closing the handle tells the server this client is done
}
