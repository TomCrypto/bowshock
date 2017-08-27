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
