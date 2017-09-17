#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>

namespace rtl {

enum class rational_mode {
  best,
  fast
};

namespace units_detail {

template <typename T> constexpr T modinv(T a);

static constexpr rtl::u8 modinv_table[16] = {
  1, 11, 13, 7, 9, 3, 5, 15
};

// @brief Computes the modular inverse of \c a modulo 2^8.
//
// @remarks \c a must be odd.
template <> constexpr rtl::u8 modinv<rtl::u8>(rtl::u8 a) {
  if (a == 1) {
    return 1;
  }

  auto x = rtl::u8{modinv_table[(a & 0b1111) >> 1]};

  x *= static_cast<rtl::u8>(rtl::i8{2}  - static_cast<rtl::i8>(x)  * a);

  return x;
}

// @brief Computes the modular inverse of \c a modulo 2^16.
//
// @remarks \c a must be odd.
template <> constexpr rtl::u16 modinv<rtl::u16>(rtl::u16 a) {
  if (a == 1) {
    return 1;
  }

  auto x = rtl::u16{modinv_table[(a & 0b1111) >> 1]};

  x *= static_cast< rtl::u8>(rtl::i8{2}  - static_cast<rtl::i8>(x)  * a);
  x *= static_cast<rtl::u16>(rtl::i16{2} - static_cast<rtl::i16>(x) * a);

  return x;
}

// @brief Computes the modular inverse of \c a modulo 2^32.
//
// @remarks \c a must be odd.
template <> constexpr rtl::u32 modinv<rtl::u32>(rtl::u32 a) {
  if (a == 1) {
    return 1;
  }

  auto x = rtl::u32{modinv_table[(a & 0b1111) >> 1]};

  x *= static_cast< rtl::u8>(rtl::i8{2}  - static_cast<rtl::i8>(x)  * a);
  x *= static_cast<rtl::u16>(rtl::i16{2} - static_cast<rtl::i16>(x) * a);
  x *= static_cast<rtl::u32>(rtl::i32{2} - static_cast<rtl::i32>(x) * a);

  return x;
}

// @brief Computes the modular inverse of \c a modulo 2^64.
//
// @remarks \c a must be odd.
template <> constexpr rtl::u64 modinv<rtl::u64>(rtl::u64 a) {
  if (a == 1) {
    return 1;
  }

  auto x = rtl::u64{modinv_table[(a & 0b1111) >> 1]};

  x *= static_cast< rtl::u8>(rtl::i8{2}  - static_cast<rtl::i8>(x)  * a);
  x *= static_cast<rtl::u16>(rtl::i16{2} - static_cast<rtl::i16>(x) * a);
  x *= static_cast<rtl::u32>(rtl::i32{2} - static_cast<rtl::i32>(x) * a);
  x *= static_cast<rtl::u64>(rtl::i64{2} - static_cast<rtl::i64>(x) * a);  

  return x;
}

template <typename U, typename UD, rational_mode mode> struct rational {
public:
  using S = typename std::make_signed<U>::type;
  using SD = typename std::make_signed<UD>::type;

  constexpr auto numerator() const { return p; }
  constexpr auto denominator() const { return q; }

  constexpr rational() {}

  constexpr rational(float x) {
    auto p_limit = std::numeric_limits<S>::max();
    auto q_limit = std::numeric_limits<U>::max();

    auto p1 = SD{0};
    auto q1 = UD{1};
    auto p2 = SD{1};
    auto q2 = UD{0};
    auto n = 0;

    do {
      auto p0 = p1;
      auto q0 = q1;
      p1 = p2;
      q1 = q2;
      n = static_cast<S>(x); // TODO: probably wrong (need integer part)
      p2 = p0 + n * p1;
      q2 = q0 + n * q1;

      if (x == n) {
        p1 = p2;
        q1 = q2;
      } else {
        x = 1 / (x - n);
      }
    } while (p2 < p_limit && q2 < q_limit && n != x);

    this->p = p1;
    this->q = q1;
  }

  constexpr rational(S p, U q) : p(p), q(q) {
    rtl::assert(q != 0, "denominator must be nonzero");

    auto pd = static_cast<SD>(this->p);
    auto qd = static_cast<UD>(this->q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);
  }

  template <typename U2, typename UD2, rational_mode mode2> constexpr rational(const rational<U2, UD2, mode2>& other) {
    using SD2 = typename std::make_signed<UD2>::type;

    if (sizeof(U) < sizeof(U2)) {
      auto pd = static_cast<SD2>(other.numerator());
      auto qd = static_cast<UD2>(other.denominator());

      while ((pd | qd) & ~std::numeric_limits<U>::max()) {
        pd >>= 1;
        qd >>= 1;
      }

      this->p = static_cast<S>(pd);
      this->q = static_cast<U>(qd);
    } else {
      this->p = static_cast<S>(other.numerator());
      this->q = static_cast<U>(other.denominator());
    }
  }

  constexpr rational(S x) : p(x), q(1) {}

  constexpr auto& operator+=(const rational<U, UD, mode>& rhs) {
    auto pd = static_cast<SD>(this->p) * static_cast<SD>(rhs.q)
            + static_cast<SD>(this->q) * static_cast<SD>(rhs.p);
    auto qd = static_cast<UD>(this->q) * static_cast<UD>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto& operator+=(const S& rhs) {
    auto pd = static_cast<SD>(this->p)
            + static_cast<SD>(this->q) * static_cast<SD>(rhs);
    auto qd = static_cast<UD>(this->q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto operator+(const rational<U, UD, mode>& rhs) const {
    auto lhs = *this;
    return lhs += rhs;
  }

  constexpr auto& operator-=(const rational<U, UD, mode>& rhs) {
    auto pd = static_cast<SD>(this->p) * static_cast<SD>(rhs.q)
            - static_cast<SD>(this->q) * static_cast<SD>(rhs.p);
    auto qd = static_cast<UD>(this->q) * static_cast<UD>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto& operator-=(const S& rhs) {
    auto pd = static_cast<SD>(this->p)
            - static_cast<SD>(this->q) * static_cast<SD>(rhs);
    auto qd = static_cast<UD>(this->q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto operator-(const rational<U, UD, mode>& rhs) const {
    auto lhs = *this;
    return lhs -= rhs;
  }

  constexpr auto& operator*=(const rational<U, UD, mode>& rhs) {
    auto pd = static_cast<SD>(this->p) * static_cast<SD>(rhs.p);
    auto qd = static_cast<UD>(this->q) * static_cast<UD>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto& operator*=(const S& rhs) {
    auto pd = static_cast<SD>(this->p) * static_cast<SD>(rhs);
    auto qd = static_cast<UD>(this->q);

    normalize(pd, qd);

    this->p = static_cast<S>(pd);
    this->q = static_cast<U>(qd);

    return *this;
  }

  constexpr auto operator*(const rational<U, UD, mode>& rhs) const {
    auto lhs = *this;
    return lhs *= rhs;
  }

  constexpr auto& operator/=(const rational<U, UD, mode>& rhs) {
    return *this *= (1 / rhs);
  }

  constexpr auto& operator/=(const S& rhs) {
    return *this * (1 / rational<U, UD, mode>{rhs});
  }

  constexpr auto operator/(const rational<U, UD, mode>& rhs) const {
    auto lhs = *this;
    return lhs /= rhs;
  }

  constexpr auto operator==(const rational<U, UD, mode>& rhs) const {
    if (mode == rtl::rational_mode::best) {
      return (this->p == rhs.p) && (this->q == rhs.q);
    } else {
      auto lhs_d = static_cast<SD>(this->p) * static_cast<SD>(rhs.q);
      auto rhs_d = static_cast<SD>(rhs.p) * static_cast<SD>(this->q);

      return lhs_d == rhs_d;
    }
  }

  constexpr auto operator!=(const rational<U, UD, mode>& rhs) const {
    return !(*this == rhs);
  }
  
  constexpr auto operator<(const rational<U, UD, mode>& rhs) const {
    auto lhs_d = static_cast<SD>(this->p) * static_cast<SD>(rhs.q);
    auto rhs_d = static_cast<SD>(this->q) * static_cast<SD>(rhs.p);

    return lhs_d < rhs_d;
  }

  constexpr auto operator>(const rational<U, UD, mode>& rhs) const {
    return rhs < *this;
  }
  
  constexpr auto operator<=(const rational<U, UD, mode>& rhs) const {
    return !(*this > rhs);
  }
  
  constexpr auto operator>=(const rational<U, UD, mode>& rhs) const {
    return !(*this < rhs);
  }

private:
  constexpr static auto gcd(UD a, UD b) {
    auto gcd = UD{1};

    while (b != 0) {
      gcd = b;
      b = a % b;
      a = gcd;
    }

    return gcd;
  }

  constexpr static auto normalize(SD& pd, UD& qd) {
    if (pd == 0) {
      qd = 1;
      return;
    }

    auto positive = true;

    if (pd < 0) {
      positive = false;
      pd *= -1;
    }

    if (mode == rtl::rational_mode::best) {
      while ((pd % 2 == 0) && (qd % 2 == 0)) {
        pd /= 2;
        qd /= 2;
      }
  
      auto inv = modinv<UD>(gcd(pd, qd));
      //auto divisor = gcd(pd, qd);

      if (inv > 1) {
        pd *= inv;
        qd *= inv;
      }

      /*
      if (divisor > 1) {
        pd /= divisor;
        qd /= divisor;
      }
      */
    }

    //while ((pd | qd) & ~std::numeric_limits<U>::max()) {
    while (pd > std::numeric_limits<S>::max() || qd > std::numeric_limits<U>::max()) {
      pd /= 2;
      qd /= 2;
    }

    if (!positive) {
      pd *= -1;
    }

    if (qd == 0) {
      qd = 1;
    }
  }

  S p{0};
  U q{1};
};

}

template <rational_mode mode> using q16 = units_detail::rational<u16, u32, mode>;
template <rational_mode mode> using q32 = units_detail::rational<u32, u64, mode>;


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
