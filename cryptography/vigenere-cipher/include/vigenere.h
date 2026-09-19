#ifndef MYCPLUS_VIGENERE_H
#define MYCPLUS_VIGENERE_H

#include <cstddef>
#include <string>

#include "encryption.h"

/*
  Writes a file encrypted with the Vigenere cipher.
  Letters are shifted by the corresponding key letter; every other
  character is written through unchanged and does not consume a key
  letter. Letter case is preserved.
*/
class VigenereEncrypt : public EncryptedFileWriter
{
public:
  VigenereEncrypt(const std::string& file = "", const std::string& code = "");

  std::string encrypt(const std::string&) override;

private:
  std::string key;
  std::size_t key_pos;   // position in the key, carried across calls
};

/*
  Reads a file encrypted with the Vigenere cipher.
  The reader is fed one word at a time, so the key position has to
  persist between calls to decrypt().
*/
class VigenereDecrypt : public EncryptedFileReader
{
public:
  VigenereDecrypt(const std::string& file = "", const std::string& code = "");

  std::string decrypt(const std::string&) override;

private:
  std::string key;
  std::size_t key_pos;
};

#endif /* MYCPLUS_VIGENERE_H */
