#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>
#include <rtl/math/rational.hpp>

namespace rtl {

template <typename _Scale,
          typename _L = std::ratio<0>, typename _M = std::ratio<0>, typename _T = std::ratio<0>,
          typename _I = std::ratio<0>, typename _O = std::ratio<0>, typename _N = std::ratio<0>,
          typename _J = std::ratio<0>, typename _B = std::ratio<0>>
struct dimension {
  using Scale = _Scale;
  using L = _L;
  using M = _M;
  using T = _T;
  using I = _I;
  using O = _O;
  using N = _N;
  using J = _J;
  using B = _B;

  template <typename other> static constexpr auto commensurate() {
    return std::ratio_equal<L, typename other::L>::value
        && std::ratio_equal<M, typename other::M>::value
        && std::ratio_equal<T, typename other::T>::value
        && std::ratio_equal<I, typename other::I>::value
        && std::ratio_equal<O, typename other::O>::value
        && std::ratio_equal<N, typename other::N>::value
        && std::ratio_equal<J, typename other::J>::value
        && std::ratio_equal<B, typename other::B>::value;
  }

  static constexpr auto is_dimensionless() {
    return std::ratio_equal<L, std::ratio<0>>::value
        && std::ratio_equal<M, std::ratio<0>>::value
        && std::ratio_equal<T, std::ratio<0>>::value
        && std::ratio_equal<I, std::ratio<0>>::value
        && std::ratio_equal<O, std::ratio<0>>::value
        && std::ratio_equal<N, std::ratio<0>>::value
        && std::ratio_equal<J, std::ratio<0>>::value
        && std::ratio_equal<B, std::ratio<0>>::value;
  }

  template <typename other> struct product {
    using value = dimension<std::ratio_multiply<Scale, typename other::Scale>,
                            std::ratio_add<L, typename other::L>,
                            std::ratio_add<M, typename other::M>,
                            std::ratio_add<T, typename other::T>,
                            std::ratio_add<I, typename other::I>,
                            std::ratio_add<O, typename other::O>,
                            std::ratio_add<N, typename other::N>,
                            std::ratio_add<J, typename other::J>,
                            std::ratio_add<B, typename other::B>>;
  };

  template <typename other> struct quotient {
    using value = dimension<std::ratio_divide<Scale, typename other::Scale>,
                            std::ratio_subtract<L, typename other::L>,
                            std::ratio_subtract<M, typename other::M>,
                            std::ratio_subtract<T, typename other::T>,
                            std::ratio_subtract<I, typename other::I>,
                            std::ratio_subtract<O, typename other::O>,
                            std::ratio_subtract<N, typename other::N>,
                            std::ratio_subtract<J, typename other::J>,
                            std::ratio_subtract<B, typename other::B>>;
  };

  template <typename other> struct conversion_factor {
    using value = std::ratio_divide<Scale, typename other::Scale>;
  };

  template <typename other> using times = typename product<other>::value;
  template <typename other> using per = typename quotient<other>::value;
};

template <typename T, typename Dimension> struct quantity {
private:
  template <typename T2, typename OtherDimension> friend struct quantity;

  template<bool b, typename X> struct block_unless { struct type { type() = delete; operator X(); }; };
  template<typename X> struct block_unless<true, X> { using type = X; };
  template<bool b, typename X> using block_unless_t = typename block_unless<b, X>::type;
  template<bool b, typename X> using block_if_t = block_unless_t<!b, X>;

public:
  constexpr explicit quantity(block_if_t<Dimension::is_dimensionless(), T> value) : value(value) {}
  constexpr quantity(block_unless_t<Dimension::is_dimensionless(), T> value) : value(value) {}

  // TODO: this might cause problems...
  /*
  constexpr operator T() const {
    static_assert(Dimension::template is_dimensionless(), "quantity is not dimensionless");

    return value;
  }
  */

  // TODO: replace this with some .as<dimension> template???

  constexpr auto scalar() const {
    static_assert(!Dimension::template is_dimensionless(), "quantity is dimensionless; use dimensionless() instead.");    

    return value;
  }

  constexpr auto dimensionless() const {
    static_assert(Dimension::template is_dimensionless(), "quantity is not dimensionless; use scalar() instead.");

    using scale = typename Dimension::template conversion_factor<dimension<std::ratio<1>>>::value;

    if constexpr (std::ratio_equal<scale, std::ratio<1>>::value) {
      return value;
    } else if constexpr (std::ratio_less<scale, std::ratio<1>>::value) {
      // scale is less than 1 - consider dividing by the inverse
      if constexpr (std::numeric_limits<i8>::min() <= scale::den / scale::num &&
                    std::numeric_limits<i8>::max() >= scale::den / scale::num) {
        return value / static_cast<i8>(scale::den / scale::num);
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::den / scale::num &&
                            std::numeric_limits<i16>::max() >= scale::den / scale::num) {
        return value / static_cast<i16>(scale::den / scale::num);
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::den / scale::num &&
                            std::numeric_limits<i32>::max() >= scale::den / scale::num) {
        return value / static_cast<i32>(scale::den / scale::num);
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::den / scale::num &&
                            std::numeric_limits<i64>::max() >= scale::den / scale::num) {
        return value / static_cast<i64>(scale::den / scale::num);
      }
    } else {
      // scale is greater than 1 - just multiply by the scale
      if constexpr (std::numeric_limits<i8>::min() <= scale::num / scale::den &&
                    std::numeric_limits<i8>::max() >= scale::num / scale::den) {
        return value * static_cast<i8>(scale::num / scale::den);
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::num / scale::den &&
                            std::numeric_limits<i16>::max() >= scale::num / scale::den) {
        return value * static_cast<i16>(scale::num / scale::den);
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::num / scale::den &&
                            std::numeric_limits<i32>::max() >= scale::num / scale::den) {
        return value * static_cast<i32>(scale::num / scale::den);
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::num / scale::den &&
                            std::numeric_limits<i64>::max() >= scale::num / scale::den) {
        return value * static_cast<i64>(scale::num / scale::den);
      }
    }
  }

  template <typename T2, typename OtherDimension>
  constexpr operator quantity<T2, OtherDimension>() const {
    static_assert(Dimension::template commensurate<OtherDimension>(), "incommensurate dimensions");

    using scale = typename Dimension::template conversion_factor<OtherDimension>::value;

    if constexpr (std::ratio_equal<scale, std::ratio<1>>::value) {
      return quantity<T2, OtherDimension>{static_cast<T2>(value)};
    } else if constexpr (std::ratio_less<scale, std::ratio<1>>::value) {
      // scale is less than 1 - consider dividing by the inverse
      if constexpr (std::numeric_limits<i8>::min() <= scale::den / scale::num &&
                    std::numeric_limits<i8>::max() >= scale::den / scale::num) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) / static_cast<i8>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i16>::max() >= scale::den / scale::num) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) / static_cast<i16>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i32>::max() >= scale::den / scale::num) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) / static_cast<i32>(scale::den / scale::num)};
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::den / scale::num &&
                           std::numeric_limits<i64>::max() >= scale::den / scale::num) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) / static_cast<i64>(scale::den / scale::num)};
      }
    } else {
      // scale is greater than 1 - just multiply by the scale
      if constexpr (std::numeric_limits<i8>::min() <= scale::num / scale::den &&
                    std::numeric_limits<i8>::max() >= scale::num / scale::den) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) * static_cast<i8>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i16>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i16>::max() >= scale::num / scale::den) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) * static_cast<i16>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i32>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i32>::max() >= scale::num / scale::den) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) * static_cast<i32>(scale::num / scale::den)};
      } else if constexpr (std::numeric_limits<i64>::min() <= scale::num / scale::den &&
                           std::numeric_limits<i64>::max() >= scale::num / scale::den) {
        return quantity<T2, OtherDimension>{static_cast<T2>(value) * static_cast<i64>(scale::num / scale::den)};
      }
    }
  }

  // TODO: need pow function

  constexpr auto operator-() const {
    return quantity<T, Dimension>{-value};
  }

  constexpr auto operator+(quantity<T, Dimension> rhs) const {
    return quantity<T, Dimension>{value + quantity<T, Dimension>{rhs}.value};
  }

  constexpr auto operator-(quantity<T, Dimension> rhs) const {
    return quantity<T, Dimension>{value - quantity<T, Dimension>{rhs}.value};
  }

  template <typename T2, typename OtherDimension> constexpr auto operator*(quantity<T2, OtherDimension> rhs) const {
    return quantity<T, typename Dimension::template product<OtherDimension>::value>{value * static_cast<T>(rhs.value)};
  }

  template <typename T2, typename OtherDimension> constexpr auto operator/(quantity<T2, OtherDimension> rhs) const {
    return quantity<T, typename Dimension::template quotient<OtherDimension>::value>{value / static_cast<T>(rhs.value)};
  }

  constexpr auto operator==(quantity<T, Dimension> rhs) const { return value == rhs.value; }
  constexpr auto operator<(quantity<T, Dimension> rhs)  const { return value < rhs.value; }
  constexpr auto operator!=(quantity<T, Dimension> rhs) const { return !(*this == rhs); }
  constexpr auto operator>(quantity<T, Dimension> rhs)  const { return rhs < *this; }
  constexpr auto operator<=(quantity<T, Dimension> rhs) const { return !(*this > rhs); }
  constexpr auto operator>=(quantity<T, Dimension> rhs) const { return !(*this < rhs); }

private:
  T value;
};

template <typename T, typename Dimension, typename T2>
constexpr auto operator*(quantity<T, Dimension> lhs, T2 rhs) {
  return lhs * quantity<T, dimension<std::ratio<1>>>{static_cast<T>(rhs)};
}

template <typename T, typename Dimension, typename T2>
constexpr auto operator*(T2 lhs, quantity<T, Dimension> rhs) {
  return rhs * quantity<T, dimension<std::ratio<1>>>{static_cast<T>(lhs)};
}

template <typename T, typename Dimension, typename T2>
constexpr auto operator/(quantity<T, Dimension> lhs, T2 rhs) {
  return lhs / quantity<T, dimension<std::ratio<1>>>{static_cast<T>(rhs)};
}

template <typename T, typename Dimension, typename T2>
constexpr auto operator/(T2 lhs, quantity<T, Dimension> rhs) {
  return rhs / quantity<T, dimension<std::ratio<1>>>{static_cast<T>(lhs)};
}

}

#define EXPAND(L, M, T, I, O, N, J, B)                      \
std::ratio<L>, std::ratio<M>, std::ratio<T>, std::ratio<I>, \
std::ratio<O>, std::ratio<N>, std::ratio<J>, std::ratio<B>

#define DIMENSIONLESS             EXPAND( 0,  0,  0,  0,  0,  0,  0,  0)
#define LENGTH                    EXPAND(+1,  0,  0,  0,  0,  0,  0,  0)
#define MASS                      EXPAND( 0, +1,  0,  0,  0,  0,  0,  0)
#define TIME                      EXPAND( 0,  0, +1,  0,  0,  0,  0,  0)
#define CURRENT                   EXPAND( 0,  0,  0, +1,  0,  0,  0,  0)
#define TEMPERATURE               EXPAND( 0,  0,  0,  0, +1,  0,  0,  0)
#define MOLE                      EXPAND( 0,  0,  0,  0,  0, +1,  0,  0)
#define CANDELA                   EXPAND( 0,  0,  0,  0,  0,  0, +1,  0)
#define INFORMATION               EXPAND( 0,  0,  0,  0,  0,  0,  0, +1)
#define VELOCITY                  EXPAND(+1, -1,  0,  0,  0,  0,  0,  0)
#define ACCELERATION              EXPAND(+1, -2,  0,  0,  0,  0,  0,  0)
#define FREQUENCY                 EXPAND( 0, -1,  0,  0,  0,  0,  0,  0)

#define DEFINE_UNIT(NAME, DIMENSION) template <typename T> using NAME = quantity<T, DIMENSION>
#define DEFINE_ABBREV(NAME, UNIT)\
  constexpr auto operator"" _##NAME(long double value) {\
    return rtl::UNIT<decltype(value)>{value};\
  }\
  constexpr auto operator"" _##NAME(unsigned long long int value) {\
    return rtl::UNIT<decltype(value)>{value};\
  }\

namespace rtl {

using dimensionless = dimension<std::ratio<1>, DIMENSIONLESS>;
using meter         = dimension<std::ratio<1>, LENGTH>;
using kilometer     = dimension<std::kilo, LENGTH>;
using gram          = dimension<std::ratio<1>, MASS>;
using kilogram      = dimension<std::kilo, MASS>;
using megagram      = dimension<std::mega, MASS>;
using tonne         = megagram;
using bit           = dimension<std::ratio<1>, INFORMATION>;
using kibibit       = dimension<std::ratio<1024>, INFORMATION>;
using kilobit       = dimension<std::kilo, INFORMATION>;
using mebibit       = dimension<std::ratio<1024 * 1024>, INFORMATION>;
using megabit       = dimension<std::mega, INFORMATION>;
using gibibit       = dimension<std::ratio<1024 * 1024 * 1024>, INFORMATION>;
using gigabit       = dimension<std::giga, INFORMATION>;

using hertz_        = dimension<std::ratio<1>, FREQUENCY>;
using kilohertz_    = dimension<std::kilo, FREQUENCY>;
using megahertz_    = dimension<std::mega, FREQUENCY>;

using nanosecond    = dimension<std::nano, TIME>;
using microsecond   = dimension<std::micro, TIME>;
using millisecond   = dimension<std::milli, TIME>;
using second        = dimension<std::ratio<1>, TIME>;
using minute        = dimension<std::ratio<60>, TIME>;
using hour          = dimension<std::ratio<3600>, TIME>;
using day           = dimension<std::ratio<3600 * 24>, TIME>;

DEFINE_UNIT(scalar, dimensionless);
DEFINE_UNIT(meters, meter);
DEFINE_UNIT(kilometers, kilometer);
DEFINE_UNIT(grams, gram);
DEFINE_UNIT(milliseconds, millisecond);
DEFINE_UNIT(seconds, second);
DEFINE_UNIT(bits, bit);
DEFINE_UNIT(hertz, hertz_);
DEFINE_UNIT(kilohertz, kilohertz_);
DEFINE_UNIT(megahertz, megahertz_);

}

DEFINE_ABBREV(meters, meters);
DEFINE_ABBREV(kilometers, kilometers);
DEFINE_ABBREV(grams, grams);
DEFINE_ABBREV(milliseconds, milliseconds);
DEFINE_ABBREV(seconds, seconds);
DEFINE_ABBREV(scalar, scalar);
DEFINE_ABBREV(Hz, hertz);
DEFINE_ABBREV(KHz, kilohertz);
DEFINE_ABBREV(MHz, megahertz);

#undef DEFINE_UNIT
#undef DEFINE_ABBREV
#undef EXPAND
#undef DIMENSIONLESS
#undef LENGTH
#undef MASS
#undef TIME
#undef CURRENT
#undef TEMPERATURE
#undef MOLE
#undef CANDELA
#undef INFORMATION
