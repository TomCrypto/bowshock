#include <rtl/platform.hpp>

namespace hal
{

extern void startup(const char* assert_message);

}

namespace rtl
{

section(".nvram") const char* assert_message;

static const char* clear_assert_message() {
  auto message = assert_message;
  assert_message = nullptr;
  return message;
}

extern "C" void rtl_startup() {
  // TODO: any other setup (stack, NVIC, CPU fault detection)
  
  hal::startup(clear_assert_message());
}



}
