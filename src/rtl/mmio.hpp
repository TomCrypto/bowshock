#pragma once

/// @file
///
/// @brief Memory-mapped IO functionality.
///
/// All memory-mapped IO registers should be accessed through this API, as it allows type-safe access to the underlying
/// IO memory regions with optional and safe bit-level masking, allowing for safe read-modify-write operations.

#include <rtl/base.hpp>
#include <rtl/assert.hpp>

namespace rtl {

/// @brief A memory-mapped IO register of a given width.
///
/// This class operates on the assumption that the bit patterns read from and written to the wrapped memory address may
/// have side effects and cannot be coalesced, optimized out or otherwise statically reasoned about. All operations are
/// carried out as read-modify-write sequences, except where the operation is independent of the register's contents.
///
/// @remarks For operations taking masks, only bits of the register where the corresponding mask bit is set to 1 *will
///          be affected* by the operation being performed, meaning they will **read** zero or **write** their current
///          value.
///
/// @remarks It is recommended to use \c 0b notation when writing masks for clarity.
template <typename T> struct mmio {
private:
  static constexpr auto all_bits = std::numeric_limits<T>::max();

public:
  static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value,
                "MMIO register type must be an unsigned integral type");

  /// @brief Wraps the given memory address as an MMIO register.
  constexpr explicit mmio(uptr word) : reg((volatile T*)word) {}

  /// @brief Clears all bits in the mask from the register. This is equivalent to \c write<mask>(all bits zero).
  ///
  /// @remarks If your mask is all-ones, use the non-templated \c clear() function instead to avoid a readback.
  template <T mask> void clear() {
    *reg &= ~mask;
  }

  /// @brief Sets all bits of the register to zero.
  void clear() {
    *reg = 0;
  }

  /// @brief Writes all bits in the mask to the register. This is equivalent to \c write<mask>(all bits one).
  ///
  /// @remarks If your mask is all-ones, use the non-templated \c set() function instead to avoid a readback.
  template <T mask> void set() {
    *reg |= mask;
  }

  /// @brief Sets all bits of the register to one.
  void set() {
    *reg = all_bits;
  }

  /// @brief Toggles the register's bits.
  template <T mask = all_bits> void toggle() {
    *reg ^= mask;
  }

  /// @brief Writes the specified bits to the register.
  ///
  /// @warning If the mask does not fully cover the bits being written, the behaviour is undefined (and an assert will
  ///          be thrown if enabled). To allow such operations, use the \c safe_write() function instead.
  template <T mask> void write(T bits) {
    rtl::assert(!(bits & ~mask), TRACE("attempted to write bits outside mask specification to mmio register"));

    *reg = bits | (*reg & ~mask);
  }

  /// @brief Writes the specified bits to the register.
  ///
  /// @remarks This function will mask the bits of the argument with the mask prior to writing them. If you know that
  ///          the argument is fully covered by the mask already, call \c write() directly to avoid this overhead.
  template <T mask> void safe_write(T bits) {
    write(bits & mask);
  }

  /// @brief Overwrites the entire register with the specified bits.
  void write(T bits) {
    *reg = bits;
  }

  static inline int foo = 42;

  /// @brief Reads out the register's bits.
  template <T mask = all_bits> T read() const {
    return *reg & mask;
  }

private:
  volatile T* reg;
};

}
