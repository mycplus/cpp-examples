// type_name.h - a readable class name from std::type_info.
//
// type_info::name() is implementation-defined: GCC and Clang return a mangled
// name ("7PolyDer"), MSVC returns "class PolyDer" or "struct PolyDer". This
// helper demangles on GCC and Clang and drops MSVC's "class "/"struct " prefix,
// so the programs print the same text everywhere.
#pragma once

#include <cstdlib>
#include <memory>
#include <string>
#include <typeinfo>

#if defined(__GNUG__)
#include <cxxabi.h>
#endif

inline std::string readable_name(const std::type_info& type)
{
    std::string name = type.name();
#if defined(__GNUG__)
    int status = 0;
    std::unique_ptr<char, void (*)(void*)> demangled(
        abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status), std::free);
    if (status == 0 && demangled)
        name = demangled.get();
#endif
    for (const char* prefix : {"class ", "struct "})
        if (name.rfind(prefix, 0) == 0)
            name.erase(0, std::string(prefix).size());
    return name;
}
