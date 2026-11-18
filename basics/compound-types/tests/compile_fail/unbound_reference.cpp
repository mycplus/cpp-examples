// Must NOT compile: a reference must be bound to an object when it is
// created. There is no "null reference" to start from.
int main()
{
    int& r;
    return r;
}
