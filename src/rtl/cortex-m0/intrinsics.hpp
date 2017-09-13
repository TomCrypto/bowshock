#pragma once

/// @file
///
/// @brief Intrinsics for the Cortex-M0 processor.

namespace rtl::intrinsics {

/// @brief Enables interrupt handling.
inline auto enable_interrupts() {
  asm volatile ("cpsie i");
}

/// @brief Disables interrupt handling.
inline auto disable_interrupts() {
  asm volatile ("cpsid i");
}

template <typename T> auto non_preemptible(T context) {
  disable_interrupts();
  context();
  enable_interrupts();
}

/// @brief Pauses execution until after at least one interrupt has been handled.
inline auto wait_for_interrupt() {
  asm volatile ("wfi");
}

/// @brief Returns whether the current execution context is privileged.
inline auto privileged() {
  return true;
}

}
