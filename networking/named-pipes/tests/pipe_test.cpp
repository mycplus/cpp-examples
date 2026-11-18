// pipe_test.cpp - runs the example programs against each other and prints a
// transcript that CTest compares with tests/expected/<scenario>.txt.
//
// usage: pipe_test <directory with the .exe files> <scenario>
//
// Each scenario uses its own pipe name (with this process's ID in it), so the
// tests can run in parallel. The transcript replaces that name with NAME.
// When several clients run at once, the order of the server's lines and the
// client numbers it assigns vary from run to run, so those scenarios replace
// the numbers with # and sort the server's lines.
#include "../src/pipe_common.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::wstring g_dir;
int g_files = 0;

std::wstring widen(const std::string& s) { return std::wstring(s.begin(), s.end()); }

struct Process {
    PROCESS_INFORMATION info{};
    std::wstring output_file;
};

// Starts dir\exe with the given arguments; stdout and stderr go to a file.
Process start(const std::string& exe, const std::vector<std::string>& args)
{
    Process p;
    p.output_file = g_dir + L"\\pipe_test_" + std::to_wstring(GetCurrentProcessId()) + L"_"
                    + std::to_wstring(++g_files) + L".txt";

    SECURITY_ATTRIBUTES inherit{sizeof inherit, nullptr, TRUE};
    UniqueHandle out(CreateFileW(p.output_file.c_str(), GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &inherit,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    std::wstring command = L"\"" + g_dir + L"\\" + widen(exe) + L".exe\"";
    for (const std::string& a : args)
        command += L" \"" + widen(a) + L"\"";

    STARTUPINFOW si{};
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = out.get();
    si.hStdError = out.get();
    if (!out.valid() || !CreateProcessW(nullptr, &command[0], nullptr, nullptr, TRUE, 0, nullptr,
                                        nullptr, &si, &p.info)) {
        std::cout << "cannot start " << exe << ": " << error_text(GetLastError()) << '\n';
        std::exit(1);
    }
    return p;
}

std::string read_output(const Process& p)
{
    // Opened with full sharing: the child process may still be writing to it.
    UniqueHandle file(CreateFileW(p.output_file.c_str(), GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    std::string text;
    char buffer[4096];
    DWORD got = 0;
    while (file.valid() && ReadFile(file.get(), buffer, static_cast<DWORD>(sizeof buffer), &got, nullptr) && got > 0)
        text.append(buffer, got);
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

// Waits up to 15 seconds and returns the exit code, or -1 after killing a hung process.
int finish(Process& p)
{
    int code = -1;
    if (WaitForSingleObject(p.info.hProcess, 15000) == WAIT_OBJECT_0) {
        DWORD exit_code = 0;
        GetExitCodeProcess(p.info.hProcess, &exit_code);
        code = static_cast<int>(exit_code);
    } else {
        TerminateProcess(p.info.hProcess, 99);
        WaitForSingleObject(p.info.hProcess, INFINITE);
    }
    CloseHandle(p.info.hThread);
    CloseHandle(p.info.hProcess);
    return code;
}

// A server is ready once it has printed its "listening" line.
void wait_until_listening(const Process& server)
{
    for (int i = 0; i < 100; ++i) {
        if (read_output(server).find("listening") != std::string::npos)
            return;
        Sleep(50);
    }
}

void replace_all(std::string& text, const std::string& from, const std::string& to)
{
    for (std::size_t at = 0; (at = text.find(from, at)) != std::string::npos; at += to.size())
        text.replace(at, from.size(), to);
}

// "client 2" becomes "client #", "instance 3" becomes "instance #".
std::string mask_numbers(std::string text)
{
    for (const std::string word : {"client ", "instance "}) {
        for (std::size_t at = 0; (at = text.find(word, at)) != std::string::npos;) {
            at += word.size();
            std::size_t end = at;
            while (end < text.size() && text[end] >= '0' && text[end] <= '9')
                ++end;
            if (end > at)
                text.replace(at, end - at, "#");
        }
    }
    return text;
}

std::string sorted_lines(const std::string& text)
{
    std::vector<std::string> lines;
    std::istringstream in(text);
    for (std::string line; std::getline(in, line);)
        lines.push_back(line);
    std::sort(lines.begin(), lines.end());
    std::string out;
    for (const std::string& line : lines)
        out += line + '\n';
    return out;
}

struct Transcript {
    std::string name;
    std::string text;
    void add(const std::string& label, int code, std::string output, bool concurrent = false)
    {
        replace_all(output, name, "NAME");
        if (concurrent)
            output = sorted_lines(mask_numbers(output));
        text += "--- " + label + " (exit " + std::to_string(code) + ")\n" + output;
    }
};

}    // namespace

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "usage: pipe_test <bin dir> <scenario>\n";
        return 2;
    }
    g_dir = widen(argv[1]);
    const std::string scenario = argv[2];
    Transcript t;
    t.name = "test-" + scenario + "-" + std::to_string(GetCurrentProcessId());
    const std::string& name = t.name;

    if (scenario == "simple") {
        // Two clients, one after the other, on the one-instance server.
        Process server = start("pipe_server", {name, "2"});
        wait_until_listening(server);
        Process c1 = start("pipe_client", {name, "hello", "named pipes"});
        const int c1_code = finish(c1);
        Process c2 = start("pipe_client", {name, "second client"});
        const int c2_code = finish(c2);
        const int s_code = finish(server);
        t.add("client 1", c1_code, read_output(c1));
        t.add("client 2", c2_code, read_output(c2));
        t.add("server", s_code, read_output(server));
    } else if (scenario == "thread" || scenario == "overlapped") {
        // Three clients at the same time.
        Process server = start(scenario == "thread" ? "thread_server" : "overlapped_server", {name, "3"});
        wait_until_listening(server);
        std::vector<Process> clients;
        for (const char* word : {"alpha", "bravo", "charlie"})
            clients.push_back(start("pipe_client", {name, word, std::string(word) + " again"}));
        std::vector<int> codes;
        for (Process& c : clients)
            codes.push_back(finish(c));
        const int s_code = finish(server);
        for (std::size_t i = 0; i < clients.size(); ++i)
            t.add("client " + std::to_string(i + 1), codes[i], read_output(clients[i]));
        t.add("server", s_code, read_output(server), true);
    } else if (scenario == "long-simple" || scenario == "long-overlapped") {
        // A 1,500-byte message: longer than the 512-byte read buffers, so both
        // ends must put the message together from several reads.
        std::string message;
        while (message.size() < 1500)
            message += "abcdefghijklmnopqrstuvwxyz0123456789";
        message.resize(1500);
        Process server = start(scenario == "long-simple" ? "pipe_server" : "overlapped_server", {name, "1"});
        wait_until_listening(server);
        Process c = start("pipe_client", {name, message});
        const int c_code = finish(c);
        const int s_code = finish(server);
        std::string c_out = read_output(c), s_out = read_output(server);
        std::string upper = make_reply(message).substr(5);    // drop "ECHO "
        for (std::string* out : {&c_out, &s_out}) {
            replace_all(*out, upper, "<1500 BYTES IN CAPITALS>");
            replace_all(*out, message, "<1500 bytes>");
        }
        t.add("client", c_code, c_out);
        // The overlapped server's client may land on any of its four instances.
        t.add("server", s_code, s_out, scenario == "long-overlapped");
    } else if (scenario == "duplicate-name") {
        // A second server must not be able to take over a name that is in use.
        Process first = start("thread_server", {name, "1"});
        wait_until_listening(first);
        Process second = start("thread_server", {name, "1"});
        const int second_code = finish(second);
        Process c = start("pipe_client", {name, "still the first server"});
        const int c_code = finish(c);
        const int first_code = finish(first);
        t.add("second server", second_code, read_output(second));
        t.add("client", c_code, read_output(c));
        t.add("first server", first_code, read_output(first));
    } else if (scenario == "no-server") {
        Process c = start("pipe_client", {name, "anyone there?"});
        const int c_code = finish(c);
        t.add("client", c_code, read_output(c));
    } else {
        std::cout << "unknown scenario " << scenario << '\n';
        return 2;
    }

    std::cout << t.text;
    for (int i = 1; i <= g_files; ++i)
        DeleteFileW((g_dir + L"\\pipe_test_" + std::to_wstring(GetCurrentProcessId()) + L"_"
                     + std::to_wstring(i) + L".txt").c_str());
    return 0;
}
