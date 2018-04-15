#include "string.hh"

typedef void (*ctor_type_t)();

ctor_type_t *start_ctors = nullptr;
ctor_type_t *end_ctors = nullptr;
ctor_type_t *init_array_start = nullptr;
ctor_type_t *init_array_end = nullptr;

extern "C" void _main(unsigned long multiboot, unsigned long magic);
extern "C" void loaderstart() {
    _main(0, 0);
}

extern "C" unsigned int get_esp() {
    return 0;
}

extern "C" void gdt_flush() {
}

extern "C" void tss_flush() {
}

extern "C" void *memcpy_opt(void *dest, void *src, size_t n) {
    return Mem::copy(dest, src,n);
}

static bool main_called = false;
extern "C" int main(int argc, char**argv)
{
    // prevent calling this again
    if (main_called) return 0;
    main_called = true;

    (void)argc;
    (void)argv;
    loaderstart();
    return 0;
}
