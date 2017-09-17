#include <rtl/platform.hpp>

// TODO: clean this up

#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/core_cmFunc.h>
#include <hal/lpc1114/headers/core_cm0.h>

[[noreturn]] void abort() {
  NVIC_SystemReset();
  while (1);
}
