// return_type_only.cpp - must not compile: overloads cannot differ only in
// their return type.
int parse(const char* text);
double parse(const char* text);

int main() { return 0; }
