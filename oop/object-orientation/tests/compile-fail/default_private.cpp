// default_private.cpp - must NOT compile: members of a class are private
// unless an access specifier says otherwise (a struct defaults to public).
class Counter {
    int count = 0;
};

int main()
{
    Counter c;
    return c.count;
}
