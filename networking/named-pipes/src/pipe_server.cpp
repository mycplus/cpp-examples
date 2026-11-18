// pipe_server.cpp - the simplest named pipe server. It creates one pipe
// instance, serves one client at a time, and answers every message.
//
// usage: pipe_server [name] [clients]    defaults: demo 1
#include "pipe_common.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    const std::string name = argc > 1 ? argv[1] : "demo";
    const int clients = argc > 2 ? std::atoi(argv[2]) : 1;
    const std::wstring path = pipe_path(name);

    // FILE_FLAG_FIRST_PIPE_INSTANCE: fail if another process already owns this
    // pipe name, so no other program can pose as this server.
    // PIPE_REJECT_REMOTE_CLIENTS: accept clients on this computer only.
    UniqueHandle pipe(CreateNamedPipeW(
        path.c_str(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
        1,          // one instance: one client at a time
        4096, 4096, // output and input buffer sizes (advisory)
        0,          // default timeout for WaitNamedPipe
        nullptr));  // default security: see "Security: Who Can Connect"
    if (!pipe.valid()) {
        std::cout << "server: CreateNamedPipe failed: " << error_text(GetLastError()) << '\n';
        return 1;
    }
    std::cout << "server: listening on pipe mycplus-" << name << std::endl;

    for (int served = 0; served < clients; ++served) {
        // Wait for a client. If one connected between CreateNamedPipe and this
        // call, ConnectNamedPipe fails with ERROR_PIPE_CONNECTED: that is success.
        if (!ConnectNamedPipe(pipe.get(), nullptr) && GetLastError() != ERROR_PIPE_CONNECTED) {
            std::cout << "server: ConnectNamedPipe failed: " << error_text(GetLastError()) << '\n';
            return 1;
        }
        std::cout << "server: client connected\n";

        std::string request;
        DWORD error = 0;
        while (read_message(pipe.get(), request, &error)) {
            std::cout << "server: received \"" << request << "\"\n";
            if (!write_message(pipe.get(), make_reply(request), &error))
                break;
        }
        if (error != ERROR_BROKEN_PIPE)            // anything else is a real failure
            std::cout << "server: read failed: " << error_text(error) << '\n';

        FlushFileBuffers(pipe.get());              // let the client read the last reply
        DisconnectNamedPipe(pipe.get());           // ready for the next client
        std::cout << "server: client disconnected\n";
    }
    return 0;
}
