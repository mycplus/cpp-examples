// dynamic_cast needs a polymorphic source type: Point has no virtual function.
struct Point { int x = 0; };
struct Point3 : Point { int z = 0; };

int main()
{
    Point3 p3;
    Point* p = &p3;
    Point3* back = dynamic_cast<Point3*>(p);   // error: Point is not polymorphic
    return back ? 0 : 1;
}
