#ifndef MYCPLUS_ENCRYPTION_H
#define MYCPLUS_ENCRYPTION_H

#include <fstream>
#include <iostream>
#include <string>

const int ALPHABET_SIZE = 26;

/*
  Writes text to a file, passing it through encrypt() on the way out.
  Subclasses override encrypt() to supply a cipher. The base class
  performs no transformation, so an EncryptedFileWriter on its own
  behaves like a plain ofstream.
*/
class EncryptedFileWriter : public std::ofstream
{
  friend EncryptedFileWriter& operator<<(EncryptedFileWriter&, const std::string&);

public:
  explicit EncryptedFileWriter(const std::string& file = "")
    : std::ofstream(file) {}

  virtual std::string encrypt(const std::string&);
};

/*
  Reads text from a file, passing it through decrypt() on the way in.
  Subclasses override decrypt(). The base class performs no
  transformation.
*/
class EncryptedFileReader : public std::ifstream
{
  friend EncryptedFileReader& operator>>(EncryptedFileReader&, std::string&);

public:
  explicit EncryptedFileReader(const std::string& file = "")
    : std::ifstream(file) {}

  virtual std::string decrypt(const std::string&);
};

EncryptedFileWriter& operator<<(EncryptedFileWriter&, const std::string&);
EncryptedFileReader& operator>>(EncryptedFileReader&, std::string&);

#endif /* MYCPLUS_ENCRYPTION_H */
