cmake_minimum_required(VERSION 3.15)

project(vigenere_cipher LANGUAGES CXX)

add_executable(vigenere_cipher
    src/encryption.cpp
    src/encryption-driver.cpp
    src/vigenere.cpp
)

target_include_directories(vigenere_cipher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(vigenere_cipher PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(vigenere_cipher PRIVATE /W4)
else()
    target_compile_options(vigenere_cipher PRIVATE -Wall -Wextra -pedantic)
endif()
