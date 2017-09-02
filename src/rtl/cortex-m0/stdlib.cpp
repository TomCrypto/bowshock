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

extern "C" void __aeabi_memcpy (void *dest, const void *src, size_t n)
{
memcpy (dest, src, n);
}
