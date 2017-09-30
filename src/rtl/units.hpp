#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>
#include <rtl/math/rational.hpp>

namespace rtl {

template <typename Me, typename Ms, typename Te, typename Ts>
struct dimension {
  using MassExponent = Me;
  using TimeExponent = Te;
  using MassScale = Ms;
  using TimeScale = Ts;

  template <typename other> static constexpr auto compatible() {
    return std::ratio_equal<MassExponent, typename other::MassExponent>::value &&
           std::ratio_equal<TimeExponent, typename other::TimeExponent>::value;
  }

  static constexpr auto dimensionless() {
    return std::ratio_equal<MassExponent, std::ratio<0>>::value &&
           std::ratio_equal<TimeExponent, std::ratio<0>>::value;
  }

  template <typename other> struct product {
    using value = dimension<std::ratio_add<MassExponent, typename other::MassExponent>,
                     std::ratio_multiply<MassScale, typename other::MassScale>,
                     std::ratio_add<TimeExponent, typename other::TimeExponent>,
                     std::ratio_multiply<TimeScale, typename other::TimeScale>>;
  };

  template <typename other> struct quotient {
    using value = dimension<std::ratio_subtract<MassExponent, typename other::MassExponent>,
                     std::ratio_divide<MassScale, typename other::MassScale>,
                     std::ratio_subtract<TimeExponent, typename other::TimeExponent>,
                     std::ratio_divide<TimeScale, typename other::TimeScale>>;
  };

  template <typename other> struct conversion_factor {
    using value = std::ratio_multiply<std::ratio_divide<MassScale, typename other::MassScale>,
                                      std::ratio_divide<TimeScale, typename other::TimeScale>>;
  };
};

// DIMENSION = "velocity", "radiance", "mass"
// QUANTITY = "gram", "meters per second", ...

// dimensions are named by types, e.g. rtl::velocity
// quantities are created by user defined literals, e.g. 50_g, 20_mps

template <typename T, typename Dimension>
struct my_quantity {
private:
  template <typename T2, typename OtherDimension> friend struct my_quantity;

  template<bool b, typename X> struct block_unless { struct type { type() = delete; operator X(); }; };
  template<typename X> struct block_unless<true, X> { using type = X; };
  template<bool b, typename X> using block_unless_t = typename block_unless<b, X>::type;
  template<bool b, typename X> using block_if_t = block_unless_t<!b, X>;

public:
  constexpr explicit my_quantity(block_if_t<Dimension::dimensionless(), T> value) : value(value) {}
  constexpr my_quantity(block_unless_t<Dimension::dimensionless(), T> value) : value(value) {}

  // TODO: this might cause problems...
  constexpr operator T() const {
    static_assert(Dimension::template dimensionless(), "quantity is not dimensionless");

    return value;
  }

  constexpr auto scalar() const {
    return value;
  }

  template <typename T2, typename OtherDimension>
  constexpr operator my_quantity<T2, OtherDimension>() const {
    static_assert(Dimension::template compatible<OtherDimension>(), "incompatible dimensions");

    using scale = typename Dimension::template conversion_factor<OtherDimension>::value;

    if constexpr (std::ratio_equal<scale, std::ratio<1>>::value) {
      return my_quantity<T2, OtherDimension>{T2{value}};
    } else if constexpr (std::ratio_less<scale, std::ratio<1>>::value) {
      // scale is less than 1 - consider dividing by the inverse
      if constexpr (std::numeric_limits<i8>::min() <= scale::den / scale::num &&
                    std::numeric_limits<i8>::max() >= scale::den / scale::num) {
        return my_quantity<T2, OtherDimension>{T2{value} / static_cast<i8>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i16>::max() >= scale::den / scale::num) {
        return my_quantity<T2, OtherDimension>{T2{value} / static_cast<i16>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i32>::max() >= scale::den / scale::num) {
        return my_quantity<T2, OtherDimension>{T2{value} / static_cast<i32>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i64>::max() >= scale::den / scale::num) {
        return my_quantity<T2, OtherDimension>{T2{value} / static_cast<i64>(scale::den / scale::num)};
      }
    } else {
      // scale is greater than 1 - just multiply by the scale
      if constexpr (std::numeric_limits<i8>::min() <= scale::num / scale::den &&
                    std::numeric_limits<i8>::max() >= scale::num / scale::den) {
        return my_quantity<T2, OtherDimension>{T2{value} * static_cast<i8>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i16>::max() >= scale::num / scale::den) {
        return my_quantity<T2, OtherDimension>{T2{value} * static_cast<i16>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i32>::max() >= scale::num / scale::den) {
        return my_quantity<T2, OtherDimension>{T2{value} * static_cast<i32>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i64>::max() >= scale::num / scale::den) {
        return my_quantity<T2, OtherDimension>{T2{value} * static_cast<i64>(scale::num / scale::den)};
      }
    }
  }

  // TODO: need pow function

  constexpr auto operator-() const {
    return my_quantity<T, Dimension>{-value};
  }

  constexpr auto operator+(my_quantity<T, Dimension> rhs) const {
    return my_quantity<T, Dimension>{value + my_quantity<T, Dimension>{rhs}.value};
  }

  constexpr auto operator-(my_quantity<T, Dimension> rhs) const {
    return my_quantity<T, Dimension>{value - my_quantity<T, Dimension>{rhs}.value};
  }

  template <typename T2, typename OtherDimension>
  constexpr auto operator*(my_quantity<T2, OtherDimension> rhs) const {
    return my_quantity<T, typename Dimension::template product<OtherDimension>::value>{value * rhs.value};
  }

  template <typename T2, typename OtherDimension>
  constexpr auto operator/(my_quantity<T2, OtherDimension> rhs) const {
    return my_quantity<T, typename Dimension::template quotient<OtherDimension>::value>{value / rhs.value};
  }

  constexpr auto operator==(my_quantity<T, Dimension> rhs) const { return value == rhs.value; }
  constexpr auto operator<(my_quantity<T, Dimension> rhs)  const { return value < rhs.value; }
  constexpr auto operator!=(my_quantity<T, Dimension> rhs) const { return !(*this == rhs); }
  constexpr auto operator>(my_quantity<T, Dimension> rhs)  const { return rhs < *this; }
  constexpr auto operator<=(my_quantity<T, Dimension> rhs) const { return !(*this > rhs); }
  constexpr auto operator>=(my_quantity<T, Dimension> rhs) const { return !(*this < rhs); }

private:
  T value;
};

using dimensionless = dimension<std::ratio<0>, std::ratio<1>, std::ratio<0>, std::ratio<1>>;
using gram = dimension<std::ratio<1>, std::ratio<1>, std::ratio<0>, std::ratio<1>>;
using second = dimension<std::ratio<0>, std::ratio<1>, std::ratio<1>, std::ratio<1>>;

template <typename T> using grams = my_quantity<T, gram>;
template <typename T> using seconds = my_quantity<T, second>;

// OLD STUFF BELOW

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

template <> constexpr rtl::q32 scale(magnitude magnitude) {
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
  return frequency{a.hz() / b.hz()};
}

constexpr frequency operator "" _KHz(unsigned long long value) {
  return frequency{static_cast<unsigned long>(value) * 1000};
}

constexpr frequency operator "" _MHz(unsigned long long value) {
  return frequency{static_cast<unsigned long>(value) * 1000000};
}

}

constexpr auto operator"" _grams(long double value) {
  return rtl::grams<decltype(value)>{value};
}

constexpr auto operator"" _grams(unsigned long long int value) {
  return rtl::grams<decltype(value)>{value};
}

constexpr auto operator"" _dimless(long double value)            {
  return rtl::my_quantity<decltype(value), rtl::dimensionless>{value};
}

constexpr auto operator"" _dimless(unsigned long long int value) {
  return rtl::my_quantity<decltype(value), rtl::dimensionless>{value};
}
