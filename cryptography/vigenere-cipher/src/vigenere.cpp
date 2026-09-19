#include <cctype>

#include "vigenere.h"

using namespace std;

namespace {

/*
  Shift one letter by one key letter.
  'A' or 'a' shifts by 0, 'B' or 'b' by 1, and so on.
  For decryption the complement (26 - shift) is added rather than the
  shift subtracted, because % in C++ keeps the sign of the dividend and
  subtracting can produce a negative index.
*/
char shift_letter(char c, char key_letter, bool encrypting)
{
  const unsigned char uc = static_cast<unsigned char>(c);
  const unsigned char uk = static_cast<unsigned char>(key_letter);

  const int base  = isupper(uc) ? 'A' : 'a';
  const int shift = tolower(uk) - 'a';
  const int off   = encrypting ? shift : ALPHABET_SIZE - shift;

  return static_cast<char>(base + (c - base + off) % ALPHABET_SIZE);
}

/*
  Apply the cipher to one chunk of text, continuing from key_pos and
  leaving key_pos ready for the next chunk.
*/
string apply(const string& text, const string& key, size_t& key_pos, bool encrypting)
{
  if (key.empty())
    return text;

  string out = text;

  for (size_t i = 0; i < out.size(); ++i) {
    const unsigned char uc = static_cast<unsigned char>(out[i]);
    if (!isalpha(uc))
      continue;

    out[i] = shift_letter(out[i], key[key_pos], encrypting);
    key_pos = (key_pos + 1) % key.size();
  }

  return out;
}

} // namespace

VigenereEncrypt::VigenereEncrypt(const string& file, const string& code)
  : EncryptedFileWriter(file), key(code), key_pos(0)
{
}

VigenereDecrypt::VigenereDecrypt(const string& file, const string& code)
  : EncryptedFileReader(file), key(code), key_pos(0)
{
}

string VigenereEncrypt::encrypt(const string& plain)
{
  return apply(plain, key, key_pos, true);
}

string VigenereDecrypt::decrypt(const string& cipher)
{
  return apply(cipher, key, key_pos, false);
}
