# Vigenère Cipher

A C++ implementation of the Vigenère cipher demonstrating inheritance,
file I/O, operator overloading, and a reusable encryption/decryption
interface.

This example accompanies a programming tutorial published on
[MYCPLUS](https://www.mycplus.com/).

## Project structure

```text
vigenere-cipher/
├── CMakeLists.txt
├── include/
│   ├── encryption.h
│   └── vigenere.h
├── src/
│   ├── encryption.cpp
│   ├── encryption-driver.cpp
│   └── vigenere.cpp
└── data/
    ├── EncryptedText.txt
    └── Example.txt
```

## Requirements

- A C++ compiler supporting C++17 or later
- CMake 3.15 or later (when using CMake)

The source also compiles with earlier C++ standards; C++17 is used here
as the repository's consistent build target.

## Build with CMake

From the `vigenere-cipher` directory:

```bash
cmake -S . -B build
cmake --build build
```

On Windows with a multi-configuration generator, the executable will
normally be under `build/Debug/` or `build/Release/`.

## Build directly with a compiler

### GCC / Clang

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    src/encryption.cpp src/vigenere.cpp src/encryption-driver.cpp \
    -Iinclude -o vigenere_cipher
```

### Microsoft Visual C++

From a Developer Command Prompt:

```bat
cl /std:c++17 /W4 /EHsc /Iinclude ^
   src\encryption.cpp src\vigenere.cpp src\encryption-driver.cpp ^
   /Fe:vigenere_cipher.exe
```

## Running the example

The program accepts these commands:

```text
encrypt [input file] [output file] [password]
decrypt [input file] [output file] [password]
quit
```

For example, from the project directory:

```text
encrypt data/Example.txt data/EncryptedText.txt PASSWORD
quit
```

The sample `Example.txt` contains:

```text
Veni vidi vici
```

With the password `PASSWORD`, the encrypted result is:

```text
Kefa rwul kiua
```

To decrypt it:

```text
decrypt data/EncryptedText.txt data/DecryptedText.txt PASSWORD
quit
```

The resulting `DecryptedText.txt` should contain:

```text
Veni vidi vici
```

## Notes

- Alphabetic characters are encrypted while non-alphabetic characters
  are preserved.
- Letter case is preserved.
- Non-alphabetic characters do not consume a key position.
- The password is treated case-insensitively.
- The example expects an alphabetic password.

## Related MYCPLUS content

See the corresponding Vigenère Cipher tutorial on
[MYCPLUS](https://www.mycplus.com/).
