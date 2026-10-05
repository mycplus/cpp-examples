#include <memory>
#include <utility>

void take_raw(int* p);
void take_unique(std::unique_ptr<int> p);

void forward_raw(int* p)                     { take_raw(p); }
void forward_unique(std::unique_ptr<int> p)  { take_unique(std::move(p)); }
