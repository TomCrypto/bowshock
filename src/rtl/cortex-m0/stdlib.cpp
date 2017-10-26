#include <rtl/platform.hpp>

extern "C" void *memcpy(void* a, const void* b, std::size_t n)
{
    uint8_t* ptrA = (uint8_t*)a;
    const uint8_t* ptrB = (const uint8_t*)b;

    while (n--) {
        *(ptrA++) = *(ptrB++);
    }

    return a;
}

extern "C" void *memset(void* a, int c, std::size_t n)
{
    uint8_t* ptrA = (uint8_t*)a;

    while (n--) {
        *(ptrA++) = static_cast<uint8_t>(c);
    }

    return a;
}

extern "C" int memcmp(const void* a, const void* b, std::size_t n)
{
    const uint8_t* ptrA = (const uint8_t*)a;
    const uint8_t* ptrB = (const uint8_t*)b;

    while (n--) {
        char x = *(ptrA++);
        char y = *(ptrB++);

        if (x < y) {
            return -1;
        } else if (x > y) {
            return 1;
        }
    }

    return 0;
}

extern "C" void __aeabi_memcpy (void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}

extern "C" void __aeabi_memset (void *dest, int c, size_t n)
{
  memset(dest, c, n);
}

extern "C" void atexit() {
  return;
}
