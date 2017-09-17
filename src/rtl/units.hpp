#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>
#include <rtl/math/rational.hpp>

namespace rtl {

/// @brief Clock frequency representation.
///
/// This object is capable of accurately representing frequencies from 1 KHz to 4 THz, in 1 KHz increments.
///
/// @remarks This is meant for describing the high frequencies typically used by peripheral clocks. For fine-grained
///          control, consider using the \c interval object.
struct frequency {
public:
  explicit constexpr frequency(unsigned long hz) : hz_(hz) {}

  auto hz() const {
    return hz_;
  }

  static auto none() {
    return frequency{0};
  }

  frequency operator*(const frequency& other) {
    return frequency{hz() * other.hz()};
  }

private:
  unsigned long hz_;
};

constexpr auto operator/(const frequency& a, const frequency& b) {
  return a.hz() / b.hz();
}

constexpr frequency operator "" _KHz(unsigned long long value) {
  return frequency{static_cast<unsigned long>(value) * 1000};
}

constexpr frequency operator "" _MHz(unsigned long long value) {
  return frequency{static_cast<unsigned long>(value) * 1000000};
}

}
