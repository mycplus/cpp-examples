#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "encryption.h"
#include "vigenere.h"

using namespace std;

static void print_menu()
{
  cout << "Enter a command:\n"
       << "encrypt [input file] [output file] [password]\n"
       << "decrypt [input file] [output file] [password]\n"
       << "quit\n";
}

/*
  Read infile, encrypt it with password, write it to outfile.
*/
static void do_encrypt(const string& infile, const string& outfile, const string& password)
{
  ifstream in(infile);
  if (!in.is_open()) {
    cout << "Could not open " << infile << " for reading\n";
    return;
  }

  ostringstream oss;
  oss << in.rdbuf();

  VigenereEncrypt out(outfile, password);
  if (!out.is_open()) {
    cout << "Could not open " << outfile << " for writing\n";
    return;
  }

  out << oss.str();
}

/*
  Read infile, decrypt it with password, write it to outfile.
  The reader hands over one word at a time, so whitespace between
  words has to be copied across separately.
*/
static void do_decrypt(const string& infile, const string& outfile, const string& password)
{
  VigenereDecrypt in(infile, password);
  if (!in.is_open()) {
    cout << "Could not open " << infile << " for reading\n";
    return;
  }

  ofstream out(outfile);
  if (!out.is_open()) {
    cout << "Could not open " << outfile << " for writing\n";
    return;
  }

  string word;
  while (in >> word) {
    out << word;

    // Copy the run of whitespace that follows this word.
    int ch;
    while ((ch = in.get()) != char_traits<char>::eof()
           && isspace(static_cast<unsigned char>(ch)))
      out << static_cast<char>(ch);

    if (ch != char_traits<char>::eof())
      in.unget();
  }
}

int main()
{
  string cmd;

  print_menu();

  while (cin >> cmd && cmd != "quit") {
    if (cmd == "encrypt" || cmd == "decrypt") {
      string infile, outfile, password;
      if (!(cin >> infile >> outfile >> password))
        break;

      if (cmd == "encrypt")
        do_encrypt(infile, outfile, password);
      else
        do_decrypt(infile, outfile, password);
    }
    else {
      cout << "Command \"" << cmd << "\" not recognized. "
           << "Use encrypt, decrypt or quit.\n";
    }
  }

  return 0;
}
