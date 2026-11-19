// dynamic_cast cannot start from void*: there is no type information to read.
struct Base { virtual ~Base() = default; };

int main()
{
    Base b;
    void* v = &b;
    Base* back = dynamic_cast<Base*>(v);   // error: void* is not a pointer to a class
    return back ? 0 : 1;
}
