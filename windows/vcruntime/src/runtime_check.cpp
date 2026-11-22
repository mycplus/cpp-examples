// runtime_check.cpp - reports which Visual C++ v14 runtimes are installed,
// the way an installer would check before deciding to run vc_redist.
//   1. The registry values the Redistributable writes (Installed, Version),
//      in both registry views.
//   2. Whether this process can load vcruntime140.dll, vcruntime140_1.dll and
//      msvcp140.dll, which copy Windows picks and its file version.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <cstdio>
#include <string>
#include <vector>

// Reads HKLM\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\<arch> in one
// registry view. Microsoft documents the 32-bit view (WOW6432Node on 64-bit
// Windows), but the two views can disagree, so the program reports both and
// the file version below is what to trust.
static void report_registry(const wchar_t* arch, REGSAM view, const char* view_name)
{
    const std::wstring path = std::wstring(L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\") + arch;
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ | view, &key) != ERROR_SUCCESS) {
        std::printf("registry %-6ls %-12s not installed\n", arch, view_name);
        return;
    }
    DWORD installed = 0, size = sizeof installed;
    RegQueryValueExW(key, L"Installed", nullptr, nullptr, reinterpret_cast<BYTE*>(&installed), &size);
    wchar_t version[64] = L"";
    size = sizeof version;
    RegQueryValueExW(key, L"Version", nullptr, nullptr, reinterpret_cast<BYTE*>(version), &size);
    RegCloseKey(key);
    std::printf("registry %-6ls %-12s Installed=%lu Version=%ls\n", arch, view_name, installed, version);
}

// Loads a DLL by name with the normal search order, then prints the path of
// the copy Windows chose and its file version. (This program is linked with
// the static runtime, so it runs even when no runtime DLL is installed.)
static void report_dll(const wchar_t* name)
{
    HMODULE dll = LoadLibraryW(name);
    if (dll == nullptr) {
        std::printf("%-20ls not found (error %lu)\n", name, GetLastError());
        return;
    }
    wchar_t path[MAX_PATH] = L"";
    GetModuleFileNameW(dll, path, MAX_PATH);
    FreeLibrary(dll);

    DWORD ignored = 0;
    const DWORD size = GetFileVersionInfoSizeW(path, &ignored);
    std::vector<unsigned char> info(size);
    VS_FIXEDFILEINFO* fixed = nullptr;
    UINT length = 0;
    if (size == 0 || !GetFileVersionInfoW(path, 0, size, info.data())
        || !VerQueryValueW(info.data(), L"\\", reinterpret_cast<void**>(&fixed), &length)) {
        std::printf("%-20ls %ls (no version information)\n", name, path);
        return;
    }
    std::printf("%-20ls %ls version %u.%u.%u.%u\n", name, path,
                static_cast<unsigned>(HIWORD(fixed->dwFileVersionMS)),
                static_cast<unsigned>(LOWORD(fixed->dwFileVersionMS)),
                static_cast<unsigned>(HIWORD(fixed->dwFileVersionLS)),
                static_cast<unsigned>(LOWORD(fixed->dwFileVersionLS)));
}

int main()
{
#if defined(_M_ARM64)
    std::puts("this process: ARM64");
#elif defined(_M_X64)
    std::puts("this process: x64");
#else
    std::puts("this process: x86");
#endif
    for (const wchar_t* arch : {L"x64", L"x86", L"arm64"}) {
        report_registry(arch, KEY_WOW64_32KEY, "32-bit view");
        report_registry(arch, KEY_WOW64_64KEY, "64-bit view");
    }
    for (const wchar_t* dll : {L"vcruntime140.dll", L"vcruntime140_1.dll", L"msvcp140.dll"})
        report_dll(dll);
}
