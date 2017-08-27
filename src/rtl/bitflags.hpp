#pragma once

//
// Provides bitwise operators for enums declared as bitflags.
//

#include <rtl/base.hpp>

namespace rtl {
  template <typename T>
  struct is_bitflags : std::false_type {};
}

template <typename T, class = typename std::enable_if<std::is_enum<T>::value && rtl::is_bitflags<T>::value>::type>
constexpr T operator |(T const& lhs, T const& rhs) {
  auto lhs_value = static_cast<typename std::underlying_type<T>::type>(lhs);
  auto rhs_value = static_cast<typename std::underlying_type<T>::type>(rhs);
  return static_cast<T>(lhs_value | rhs_value);
}

template <typename T, class = typename std::enable_if<std::is_enum<T>::value && rtl::is_bitflags<T>::value>::type>
T operator |=(T& lhs, T const& rhs) {
  return lhs = lhs | rhs;
}
