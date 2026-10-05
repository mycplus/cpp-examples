// allocations.cpp - counts heap allocations made by std::make_shared and by
// std::shared_ptr<T>(new T), and shows when each one's memory is returned.
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>

// --- an event log that does not itself allocate --------------------------
struct Event { const char* what; std::size_t bytes; };
static Event g_log[64];
static int g_count = 0;
static bool g_tracking = false;

static void log_event(const char* what, std::size_t bytes = 0)
{
    if (g_tracking && g_count < 64)
        g_log[g_count++] = {what, bytes};
}

void* operator new(std::size_t n)
{
    log_event("operator new", n);
    if (void* p = std::malloc(n))
        return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { log_event("operator delete"); std::free(p); }
void operator delete(void* p, std::size_t) noexcept { log_event("operator delete"); std::free(p); }

// --- the object being shared ---------------------------------------------
struct Payload {
    ~Payload() { log_event("~Payload()"); }
    long data[4] = {};            // 32 bytes
};

static void run(const char* title, std::shared_ptr<Payload> (*create)())
{
    g_count = 0;
    g_tracking = true;
    {
        std::shared_ptr<Payload> owner = create();
        log_event("-- created; take a weak_ptr");
        std::weak_ptr<Payload> watcher = owner;
        log_event("-- owner.reset()");
        owner.reset();
        log_event("-- weak_ptr leaves scope");
    }
    g_tracking = false;

    std::printf("%s\n", title);
    for (int i = 0; i < g_count; ++i) {
        if (g_log[i].bytes)
            std::printf("  %-28s %zu bytes\n", g_log[i].what, g_log[i].bytes);
        else
            std::printf("  %s\n", g_log[i].what);
    }
}

int main()
{
    run("std::make_shared<Payload>()",
        [] { return std::make_shared<Payload>(); });
    run("std::shared_ptr<Payload>(new Payload)",
        [] { return std::shared_ptr<Payload>(new Payload); });
}
