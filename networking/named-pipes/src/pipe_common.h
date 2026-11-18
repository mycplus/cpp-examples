// pipe_common.h - small helpers shared by the named pipe examples:
// a RAII wrapper for Windows handles, readable error messages, and the
// conversion from a short pipe name to the full \\.\pipe\ path.
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>
#include <utility>

// Owns one HANDLE and closes it exactly once. Move-only, like std::unique_ptr.
class UniqueHandle {
public:
    UniqueHandle() = default;
    explicit UniqueHandle(HANDLE h) : h_(h) {}
    ~UniqueHandle() { reset(); }
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;
    UniqueHandle(UniqueHandle&& other) noexcept : h_(std::exchange(other.h_, INVALID_HANDLE_VALUE)) {}
    UniqueHandle& operator=(UniqueHandle&& other) noexcept
    {
        if (this != &other) {
            reset();
            h_ = std::exchange(other.h_, INVALID_HANDLE_VALUE);
        }
        return *this;
    }

    HANDLE get() const { return h_; }
    bool valid() const { return h_ != INVALID_HANDLE_VALUE && h_ != nullptr; }
    void reset()
    {
        if (valid())
            CloseHandle(h_);
        h_ = INVALID_HANDLE_VALUE;
    }

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

// "error 5 (ERROR_ACCESS_DENIED)". The symbolic names are printed instead of
// FormatMessage text, because that text is translated on non-English Windows.
inline std::string error_text(DWORD code)
{
    const char* name = nullptr;
    switch (code) {
    case ERROR_FILE_NOT_FOUND:     name = "ERROR_FILE_NOT_FOUND"; break;
    case ERROR_ACCESS_DENIED:      name = "ERROR_ACCESS_DENIED"; break;
    case ERROR_BROKEN_PIPE:        name = "ERROR_BROKEN_PIPE"; break;
    case ERROR_PIPE_BUSY:          name = "ERROR_PIPE_BUSY"; break;
    case ERROR_NO_DATA:            name = "ERROR_NO_DATA"; break;
    case ERROR_PIPE_CONNECTED:     name = "ERROR_PIPE_CONNECTED"; break;
    case ERROR_MORE_DATA:          name = "ERROR_MORE_DATA"; break;
    case ERROR_SEM_TIMEOUT:        name = "ERROR_SEM_TIMEOUT"; break;
    case ERROR_IO_PENDING:         name = "ERROR_IO_PENDING"; break;
    default: break;
    }
    std::string text = "error " + std::to_string(code);
    if (name != nullptr)
        text += std::string(" (") + name + ")";
    return text;
}

// "demo" becomes L"\\.\pipe\mycplus-demo". The prefix keeps the examples'
// pipes apart from other programs' pipes on the same machine.
inline std::wstring pipe_path(const std::string& name)
{
    std::wstring path = L"\\\\.\\pipe\\mycplus-";
    for (char c : name)
        path += static_cast<wchar_t>(static_cast<unsigned char>(c));
    return path;
}

// Reads one whole message from a message-mode pipe. Returns false when the
// other end has closed the pipe (or on any other error, stored in *error).
inline bool read_message(HANDLE pipe, std::string& message, DWORD* error)
{
    message.clear();
    char buffer[512];
    for (;;) {
        DWORD got = 0;
        if (ReadFile(pipe, buffer, static_cast<DWORD>(sizeof buffer), &got, nullptr)) {
            message.append(buffer, got);
            return true;                       // the message is complete
        }
        const DWORD e = GetLastError();
        if (e == ERROR_MORE_DATA) {            // the message is longer than the buffer
            message.append(buffer, got);
            continue;
        }
        *error = e;
        return false;
    }
}

inline bool write_message(HANDLE pipe, const std::string& message, DWORD* error)
{
    DWORD written = 0;
    if (!WriteFile(pipe, message.data(), static_cast<DWORD>(message.size()), &written, nullptr)
        || written != message.size()) {
        *error = GetLastError();
        return false;
    }
    return true;
}

// The reply every server sends: the request in capitals, so a test can tell
// which reply belongs to which request.
inline std::string make_reply(const std::string& request)
{
    std::string reply = "ECHO ";
    for (char c : request)
        reply += (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
    return reply;
}
