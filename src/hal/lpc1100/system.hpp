#pragma once

/// @file
///
/// @brief System control utilities for the LPC1100 series microcontrollers.

// TODO: for deep sleep, assume a wakeup translates to a "power on" event (?) and let the application detect deep sleep
// wakeups by storing their data in their own EEPROM or proper NVRAM provided by the HAL (so that recovery from sleep
// is not a function provided by the HAL, although entering sleep might be).

#include <rtl/base.hpp>

namespace hal::lpc1100
{

/// @brief All possible events which could lead to a reset.
enum class reset_event {
  unknown,              ///< The reset event cannot be determined, this indicates software or hardware failure
  power_on,             ///< The device has been powered up
  external,             ///< The device has been reset externally through its RESET function
  brownout,             ///< The on-board brown-out detector detected a voltage sag and reset the device
  watchdog,             ///< The watchdog peripheral was not fed and reset the device
  assert,               ///< An assertion failed and the device was reset
  abort,                ///< Execution was aborted by software
  fault,                ///< A CPU fault occurred and the device was reset
  software              ///< A software reset was initiated
};

namespace
{

struct assert_reset_info {
  const char* message;
};

struct software_reset_info {
  rtl::u32 data[4];
};

}

/// @brief Context structure holding a reset event and additional information about it.
struct reset_context {
  reset_event event;

  union {
    assert_reset_info assert;
    software_reset_info software;
  };
};

}

/// @brief This function must be implemented by the application and is called when the device comes out of reset.
[[noreturn]] extern void main(const hal::lpc1100::reset_context& context);
