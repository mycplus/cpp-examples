# Vigenère Cipher

A C++ implementation of the Vigenère cipher demonstrating inheritance, file I/O, operator overloading, and a reusable encryption/decryption interface.

This example accompanies a programming tutorial published on [MYCPLUS](https://www.mycplus.com/).

[![C++ Build](https://github.com/mycplus/cpp-examples/actions/workflows/cpp-build.yml/badge.svg)](https://github.com/mycplus/cpp-examples/actions/workflows/cpp-build.yml)

## Project Structure

```text
vigenere-cipher/
├── CMakeLists.txt
├── README.md
│
├── include/
│   ├── encryption.h
│   └── vigenere.h
│
├── src/
│   ├── encryption.cpp
│   ├── encryption-driver.cpp
│   └── vigenere.cpp
│
└── data/
    ├── EncryptedText.txt
    └── Example.txt
```

### Source files

* `encryption.h` / `encryption.cpp` — base encrypted file reader and writer classes.
* `vigenere.h` / `vigenere.cpp` — Vigenère encryption and decryption implementation.
* `encryption-driver.cpp` — command-line driver program.

### Data files

* `Example.txt` — sample plaintext input.
* `EncryptedText.txt` — sample encrypted output.

## Requirements

The example is written in standard C++ and is currently built and tested with:

* GCC
* Clang
* Microsoft Visual C++

The automated GitHub Actions build uses C++17.

## Build with CMake

From the `vigenere-cipher` directory:

```bash
cmake -S . -B build
cmake --build build
```

On Windows with a multi-configuration generator, the executable will normally be located under `build/Debug/` or `build/Release/`.

## Build with GCC

```bash
g++ -std=c++17 -Wall -Wextra -pedantic \
    src/encryption.cpp \
    src/vigenere.cpp \
    src/encryption-driver.cpp \
    -Iinclude \
    -o vigenere_cipher
```

Run:

```bash
./vigenere_cipher
```

## Build with Clang

```bash
clang++ -std=c++17 -Wall -Wextra -pedantic \
    src/encryption.cpp \
    src/vigenere.cpp \
    src/encryption-driver.cpp \
    -Iinclude \
    -o vigenere_cipher
```

Run:

```bash
./vigenere_cipher
```

## Build with Microsoft Visual C++

From a Visual Studio Developer Command Prompt:

```bat
cl /std:c++17 /W4 /EHsc /Iinclude ^
   src\encryption.cpp ^
   src\vigenere.cpp ^
   src\encryption-driver.cpp ^
   /Fe:vigenere_cipher.exe
```

Run:

```bat
vigenere_cipher.exe
```

## Using the Program

The program accepts the following commands:

```text
encrypt [input file] [output file] [password]
decrypt [input file] [output file] [password]
quit
```

For example:

```text
encrypt data/Example.txt data/EncryptedText.txt PASSWORD
```

The sample `Example.txt` contains:

```text
Veni vidi vici
```

Using the password `PASSWORD`, the encrypted result is:

```text
Kefa rwul kiua
```

To decrypt the encrypted file:

```text
decrypt data/EncryptedText.txt data/DecryptedText.txt PASSWORD
```

The resulting `DecryptedText.txt` should contain:

```text
Veni vidi vici
```

## How the Example Works

The project defines generic encrypted file reader and writer classes and derives Vigenère-specific classes from them.

`VigenereEncrypt` applies the Vigenère transformation when text is written, while `VigenereDecrypt` reverses the transformation when text is read.

The implementation:

* preserves the case of alphabetic characters;
* leaves non-alphabetic characters unchanged;
* does not advance the key position for non-alphabetic characters;
* treats the key letters case-insensitively;
* maintains the key position across successive read/write operations.

The example expects an alphabetic password.

## Automated Testing

Every push to the `main` branch and every pull request targeting `main` is automatically tested using GitHub Actions.

The project is compiled and tested with:

* GCC on Ubuntu
* Clang on Ubuntu
* Microsoft Visual C++ on Windows

The CI workflow performs both compilation and a functional encryption/decryption test. The decrypted output must match the original input for the build to pass.

[View the latest GitHub Actions results](https://github.com/mycplus/cpp-examples/actions)

## Related MYCPLUS Content

See the corresponding Vigenère Cipher tutorial on [MYCPLUS](https://www.mycplus.com/).

## License

This example is provided under the MIT License.
