#include <rtl/platform.hpp>

namespace hal
{

[[noreturn]] extern void startup(const char* assert_message);

}

namespace rtl
{

namespace nv {

section(".nvram") const char* assert_message;

}

static const char* clear_assert_message() {
  auto message = nv::assert_message;
  nv::assert_message = nullptr;
  return message;
}

extern "C" [[noreturn]] void rtl_startup() {
  // TODO: any other setup (stack, NVIC, CPU fault detection)
  
  hal::startup(clear_assert_message());
}

}
