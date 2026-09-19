#include "encryption.h"

using namespace std;

/*
  Output function for EncryptedFileWriter.
  Encrypts the given string and writes it to the file.
  Calls the subclass's encrypt() when overridden.
  Writes to cout if the file is not open.
*/
EncryptedFileWriter& operator<<(EncryptedFileWriter& efw, const string& str)
{
  if (!efw.is_open())
    cout << efw.encrypt(str);
  else
    static_cast<ofstream&>(efw) << efw.encrypt(str);

  return efw;
}

/*
  Input function for EncryptedFileReader.
  Reads one whitespace-delimited word, decrypts it, and updates str.
  Reads from cin if the file is not open.
*/
EncryptedFileReader& operator>>(EncryptedFileReader& efr, string& str)
{
  if (!efr.is_open())
    cin >> str;
  else
    static_cast<ifstream&>(efr) >> str;

  str = efr.decrypt(str);
  return efr;
}

/*
  Base class transformation: none.
  A subclass supplies the cipher by overriding this.
*/
string EncryptedFileWriter::encrypt(const string& plain)
{
  return plain;
}

/*
  Base class transformation: none.
*/
string EncryptedFileReader::decrypt(const string& cipher)
{
  return cipher;
}
