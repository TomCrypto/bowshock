#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>
#include <rtl/math/rational.hpp>

namespace rtl {

enum class quantity {
  frequency,
  mass
};

enum class magnitude {
  n,
  u,
  m,
  i,
  K,
  M,
  G,
  T
};

template <typename F> constexpr F scale(magnitude magnitude);
template <> constexpr float scale(magnitude magnitude) {
  switch (magnitude) {
    case magnitude::n:
      return 1e-9f;
    case magnitude::u:
      return 1e-6f;
    case magnitude::m:
      return 1e-3f;
    case magnitude::i:
      return 1e-0f;
    case magnitude::K:
      return 1e+3f;
    case magnitude::M:
      return 1e+6f;
    case magnitude::G:
      return 1e+9f;
    case magnitude::T:
      return 1e+12f;
    default:
      abort();
  }
}

template <> constexpr rtl::q32<rational_mode::fast> scale(magnitude magnitude) {
  switch (magnitude) {
    case magnitude::n:
      return 1e-9;
    case magnitude::u:
      return 1e-6;
    case magnitude::m:
      return 1e-3;
    case magnitude::i:
      return 1e-0;
    case magnitude::K:
      return 1e+3;
    case magnitude::M:
      return 1e+6;
    case magnitude::G:
      return 1e+9;
    default:
      abort();
  }
}

template <typename F, quantity quantity, magnitude magnitude> struct unit {
  constexpr unit() {}
  constexpr unit(F value) : value_(value) {}

  template <typename F2, enum magnitude magnitude2> constexpr unit(const unit<F2, quantity, magnitude2>& other)
    : value_((other.value() * scale<F2>(magnitude2)) / scale<F>(magnitude)) {}

  constexpr operator F() const { return value_; }
  //constexpr operator F&() { return value_; }

  //constexpr F& value() { return value_; }
  constexpr F value() const { return value_; }

private:
  F value_{};
};


template <typename F> struct MHz : public unit<F, quantity::frequency, magnitude::M> {};
template <typename F> struct KHz : public unit<F, quantity::frequency, magnitude::K> {};
template <typename F> struct Hz : public unit<F, quantity::frequency, magnitude::i> {};
template <typename F> struct Kg : public unit<F, quantity::mass, magnitude::K> {};

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
