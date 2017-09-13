/// @file
///
/// @brief 6x6 LED Matrix firmware for LQFP48 LPC1115.

#include <hal/lpc1100/system.hpp>

namespace dev = hal::lpc1100;

[[noreturn]] void run_failed() {
  // stuff

  while (true);
}

struct frame {
  uint8_t intensities[6][6];
};

[[noreturn]] void run_normal() {
  // 

  while (true);
}

[[noreturn]] void main(const dev::reset_context& context) {
  switch (context.event) {
    case dev::reset_event::fault:
    case dev::reset_event::assert:
    case dev::reset_event::abort:
      run_failed();
      break;
    default:
      run_normal();
  }
}
