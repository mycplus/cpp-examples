// Must NOT compile: an array element must be an object, and a reference is
// not an object, so there are no arrays of references. Use an array of
// pointers or of std::reference_wrapper instead.
int main()
{
    int x = 1;
    int y = 2;
    int& refs[2] = {x, y};
    return refs[0];
}
