#pragma once

// TODO: remove
#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/hdr_syscon.h>

/// @file
///
/// @brief Interrupt control for the LPC1100 series microcontrollers.

#include <rtl/assert.hpp>

namespace hal::lpc1100 {

namespace interrupt {

enum class type {
  uart = 21
};

inline void enable(type type) {
  NVIC_EnableIRQ(static_cast<IRQn>(type));
}

inline void disable(type type) {
  NVIC_DisableIRQ(static_cast<IRQn>(type));
  __DSB(); // TODO: ??
  __ISB();
}

// TODO: priority functions

using handler_t = void (*)();

namespace handlers
{

extern "C" void rtl_init(void);

inline void hard_fault(void)
{
    rtl::assert(false, TRACE("hard fault handler hit"));
}

inline void default_(void)
{
    rtl::assert(false, TRACE("default handler hit"));
}

extern void uart(void);

extern "C" const char __LD_STACK_TOP;

static inline interrupt::handler_t vectors[] __attribute__((used)) section(".vect") = {
  (interrupt::handler_t)(&__LD_STACK_TOP),    // Main stack end address
  interrupt::handlers::rtl_init,                           // Exception: Reset
  nullptr,
  interrupt::handlers::hard_fault,                  // All class of fault
  nullptr,            // Reserved 0x10
  nullptr,            // Reserved 0x14
  nullptr,            // Reserved 0x18
  nullptr,            // Reserved 0x1C
  nullptr,            // Reserved 0x20
  nullptr,            // Reserved 0x24
  nullptr,            // Reserved 0x28
  interrupt::handlers::default_,                        // System service call via SWI instruction
  nullptr,            // Reserved 0x30
  nullptr,            // Reserved 0x34
  nullptr,
  interrupt::handlers::default_,                         // System tick timer
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_0 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_1 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_2 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_3 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_4 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_5 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_6 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_7 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_8 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO0_9 interrupt
  interrupt::handlers::default_,          // Start logic wake-up from PIO0_10 interrupt
  interrupt::handlers::default_,          // Start logic wake-up from PIO0_11 interrupt
  interrupt::handlers::default_,           // Start logic wake-up from PIO1_0 interrupt
  interrupt::handlers::default_,                     // C_CAN interrupt (LPC11C12/C14 only)
  interrupt::handlers::default_,                    // SPI/SSP1 interrupt
  interrupt::handlers::default_,                     // I2C interrupt
  interrupt::handlers::default_,               // CT16B0 (16-bit Timer0) interrupt
  interrupt::handlers::default_,               // CT16B1 (16-bit Timer1) interrupt
  interrupt::handlers::default_,               // CT32B0 (32-bit Timer0) interrupt
  interrupt::handlers::default_,               // CT32B1 (32-bit Timer1) interrupt
  interrupt::handlers::default_,                    // SPI/SSP0 interrupt
  interrupt::handlers::uart,                    // UART interrupt
  nullptr,            // Reserved 0x98
  nullptr,            // Reserved 0x9C
  interrupt::handlers::default_,                     // A/D Converter interrupt
  interrupt::handlers::default_,                     // Watchdog timer interrupt
  interrupt::handlers::default_,                     // Brown Out Detect interrupt
  nullptr,            // Reserved 0xAC
  interrupt::handlers::default_,                 // PIO INT3 interrupt
  interrupt::handlers::default_,                 // PIO INT2 interrupt
  interrupt::handlers::default_,                 // PIO INT1 interrupt
  interrupt::handlers::default_                 // PIO INT0 interrupt
};

}

}

}
