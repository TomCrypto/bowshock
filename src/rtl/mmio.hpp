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

namespace detail {

template <typename T> static constexpr auto all_bits = std::numeric_limits<T>::max();

template <rtl::uptr address, typename T> struct io {
public:
  static constexpr auto all_bits_set = std::numeric_limits<T>::max();

  static_assert(std::is_unsigned<T>::value && std::is_integral<T>::value,
                "MMIO register type must be an unsigned integral type");

  static auto& memory() {
    return *reinterpret_cast<volatile T*>(address);
  }
};

}

template <rtl::uptr address, typename T> struct mmio_wo {
public:
  /// @brief Clears all bits in the mask from the register. This is equivalent to \c write<mask>(all bits zero).
  ///
  /// @remarks If your mask is all-ones, use the non-templated \c clear() function instead to avoid a readback.
  template <T mask> static auto clear() {
    detail::io<address, T>::memory() &= ~mask;
  }

  /// @brief Sets all bits of the register to zero.
  static auto clear() {
    detail::io<address, T>::memory() = 0;
  }

  /// @brief Writes all bits in the mask to the register. This is equivalent to \c write<mask>(all mask bits one).
  ///
  /// @remarks If your mask is all-ones, use the non-templated \c set() function instead to avoid a readback.
  template <T mask> static auto set() {
    detail::io<address, T>::memory() |= mask;
  }

  /// @brief Sets all bits of the register to one.
  static auto set() {
    set<detail::io<address, T>::all_bits_set>();
  }

  /// @brief Toggles the register's bits.
  template <T mask = detail::io<address, T>::all_bits_set> static auto toggle() {
    detail::io<address, T>::memory() ^= mask;
  }

  /// @brief Writes the specified bits to the register.
  ///
  /// @warning If the mask does not fully cover the bits being written, the behaviour is undefined (and an assert will
  ///          be thrown if enabled). To allow such operations, use the \c safe_write() function instead.
  template <T mask> static auto write(T bits) {
    rtl::assert(!(bits & ~mask), TRACE("attempted to write bits outside mask specification to mmio register"));

    detail::io<address, T>::memory() = bits | (detail::io<address, T>::memory() & ~mask);
  }

  /// @brief Writes the specified bits to the register.
  ///
  /// @remarks This function will mask the bits of the argument with the mask prior to writing them. If you know that
  ///          the argument is fully covered by the mask already, call \c write() directly to avoid this overhead.
  template <T mask> static auto safe_write(T bits) {
    detail::io<address, T>::memory() = (bits & mask) | (detail::io<address, T>::memory() & ~mask);
  }

  /// @brief Overwrites the entire register with the specified bits.
  static auto write(T bits) {
    detail::io<address, T>::memory() = bits;
  }

  /// @brief Clears the given bit in the register.
  template <std::size_t bit> static auto clear_bit() {
    static_assert(bit < sizeof(T) * 8, "bit out of range");
    clear<1 << bit>();
  }

  /// @brief Sets the given bit in the register.
  template <std::size_t bit> static auto set_bit() {
    static_assert(bit < sizeof(T) * 8, "bit out of range");
    set<1 << bit>();
  }

  /// @brief Toggles the given bit in the register.
  template <std::size_t bit> static auto toggle_bit() {
    static_assert(bit < sizeof(T) * 8, "bit out of range");
    toggle<1 << bit>();
  }
};

template <rtl::uptr address, typename T> struct mmio_ro {
public:
  /// @brief Reads out the register's bits.
  template <T mask = detail::io<address, T>::all_bits_set> static auto read() {
    return detail::io<address, T>::memory() & mask;
  }

  /// @brief Returns whether any of the bits in the mask are set.
  template <T mask = detail::io<address, T>::all_bits_set> static auto any() {
    return (detail::io<address, T>::memory() & mask) != 0;
  }

  /// @brief Returns whether all of the bits in the mask are set.
  template <T mask = detail::io<address, T>::all_bits_set> static auto all() {
    return (detail::io<address, T>::memory() & mask) == mask;
  }

  /// @brief Returns whether none of the bits in the mask are set.
  template <T mask = detail::io<address, T>::all_bits_set> static auto none() {
    return (detail::io<address, T>::memory() & mask) == 0;
  }

  /// @brief Reads the given bit in the register.
  template <std::size_t bit> static auto read_bit() {
    static_assert(bit < sizeof(T) * 8, "bit out of range");
    return all<1 << bit>();
  }
};

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
template <rtl::uptr address, typename T> struct mmio_rw : public mmio_ro<address, T>, mmio_wo<address, T> {};

/// @brief Shortland alias of mmio_rw.
template <rtl::uptr address, typename T> using mmio = mmio_rw<address, T>;

}
