#pragma once

/// @file
///
/// @brief Debugging utilities.

#include <rtl/base.hpp>

namespace sys {

namespace detail {

auto write_str(const char* buf) {
  return [buf, index = std::size_t{0}](rtl::u8& data) mutable {
    if (buf[index] == '\0') {
      return true;
    }
    
    data = buf[index++];
    return false;
  };
}

auto write_int(rtl::i64 n) {
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

  return write_str(buffer + pos + 1);
}

}

/// @brief Base definition for a debug formatter.
///
/// @remarks Additional specializations over the \c F type with custom format strings can be introduced.
template <typename F> auto debug_formatter(const std::pair<const char*, F>& fragment);

/// @brief String debug formatter.
template <> auto debug_formatter<const char*>(const std::pair<const char*, const char*>& fragment) {
  return detail::write_str(fragment.second);
}

template <> auto debug_formatter<int>(const std::pair<const char*, int>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<unsigned int>(const std::pair<const char*, unsigned int>& fragment) {
  return detail::write_int(fragment.second);
}

template <> auto debug_formatter<rtl::i64>(const std::pair<const char*, rtl::i64>& fragment) {
  return detail::write_int(fragment.second);
}

template <typename T, typename F>
auto debug(T& output, const std::pair<const char*, F>& fragment) {
  output.write(debug_formatter(fragment)).wait();
}

template <typename T, typename F, typename... Args>
auto debug(T& output, const std::pair<const char*, F>& fragment, Args&&... rest) {
  // TODO: need to handle errors somehow!!
  output.write(debug_formatter(fragment)).wait();
  debug(output, std::forward<Args>(rest)...);
}

}
