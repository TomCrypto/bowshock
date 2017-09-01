#include <hal/lpc1100/system.hpp>

#include <rtl/mmio.hpp>

namespace hal::lpc1100
{

static reset_context determine_reset_event() {
  auto reset_status = rtl::mmio<rtl::u32>(0x40048030);
  auto reset_bits = reset_status.template read<0b11111>();
  reset_status.template set<0b11111>();

   if (reset_bits & (1 << 0)) {
     return reset_context{reset_event::power_on, {}};
   } else if (reset_bits & (1 << 1)) {
     return reset_context{reset_event::external, {}};
   } else if (reset_bits & (1 << 2)) {
     return reset_context{reset_event::watchdog, {}};
   } else if (reset_bits & (1 << 3)) {
    return reset_context{reset_event::brownout, {}};
   } else if (reset_bits & (1 << 4)) {
     return reset_context{reset_event::software, {}};
   } else {
     return reset_context{reset_event::unknown, {}};
   }
}

[[noreturn]] static void startup(const char* message) {
  auto context = determine_reset_event();

  if (context.event == reset_event::software) {
    if (message != nullptr) {
      context.event = reset_event::assert;
      context.assert.message = message;
    }
  }

  main(context);
}

}

namespace hal
{

[[noreturn]] void startup(const char* message) {
  hal::lpc1100::startup(message);
}

}
