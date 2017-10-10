#pragma once

/// @file
///
/// @brief Data formatting utilities.

#include <rtl/base.hpp>
#include <rtl/fiber/algorithm.hpp>

namespace sys {

namespace detail {

inline auto format_str(const char* buf) {
  return [buf, index = std::size_t{0}](rtl::u8& data) mutable {
    if (buf[index] == '\0') {
      return rtl::waitable::status::complete;
    }
    
    data = buf[index++];
    return rtl::waitable::status::pending;
  };
}

inline auto format_int(rtl::i64 n) {
  static char buffer[32] = {};
  std::size_t pos = 31;

  auto negative = (n < 0);
  if (negative) {
    n = -n;
  }

  buffer[pos--] = '\0';

  if (n == 0) {
    buffer[pos--] = '0';
  } else while (n != 0) {
    buffer[pos--] = '0' + (n % 10);
    n /= 10;
  }

  if (negative) {
    buffer[pos--] = '-';
  }

  return format_str(buffer + pos + 1);
}

}

/// @brief Base definition for a formatter.
///
/// @remarks Additional specializations over the \c T type with custom format strings can be introduced.
template <typename T> auto formatter(const std::pair<const char*, T>& fragment);

/// @brief String debug formatter.
template <> auto formatter<const char*>(const std::pair<const char*, const char*>& fragment) {
  return detail::format_str(fragment.second);
}

template <> auto formatter<unsigned int>(const std::pair<const char*, unsigned int>& fragment) {
  return detail::format_int(fragment.second);
}

template <> auto formatter<rtl::i16>(const std::pair<const char*, rtl::i16>& fragment) {
  return detail::format_int(fragment.second);
}

template <> auto formatter<rtl::u16>(const std::pair<const char*, rtl::u16>& fragment) {
  return detail::format_int(fragment.second);
}

template <> auto formatter<rtl::i32>(const std::pair<const char*, rtl::i32>& fragment) {
  return detail::format_int(fragment.second);
}

template <> auto formatter<rtl::i64>(const std::pair<const char*, rtl::i64>& fragment) {
  return detail::format_int(fragment.second);
}

namespace detail {

template <typename T> auto formatter_generator(std::pair<const char*, T> fragment) {
  return [fragment{std::move(fragment)}]() { return formatter(fragment); };
}

}

/// @brief Formatting function.
template <typename... Args> auto format(Args&&... args) {
  return rtl::sequence(detail::formatter_generator(std::forward<Args>(args))...);
}

}
