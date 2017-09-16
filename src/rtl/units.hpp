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

template <typename T, typename D, rational_mode mode> struct rational {
public:
  constexpr rational(T p, T q) : p(p), q(q) {
    rtl::assert(q != 0, "denominator must be nonzero");

    auto pd = static_cast<D>(this->p);
    auto qd = static_cast<D>(this->q);

    normalize(pd, qd);

    this->p = pd;
    this->q = qd;
  }

  constexpr rational(T x) : p(x), q(1) {}

  auto& operator+=(const rational<T, D, mode>& rhs) {
    auto pd = static_cast<D>(this->p) * static_cast<D>(rhs.q)
            + static_cast<D>(this->q) * static_cast<D>(rhs.p);
    auto qd = static_cast<D>(this->q) * static_cast<D>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto& operator+=(const T& rhs) {
    auto pd = static_cast<D>(this->p)
            + static_cast<D>(this->q) * static_cast<D>(rhs);
    auto qd = static_cast<D>(this->q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto operator+(const rational<T, D, mode>& rhs) const {
    auto lhs = *this;
    return lhs += rhs;
  }

  auto& operator-=(const rational<T, D, mode>& rhs) {
    auto pd = static_cast<D>(this->p) * static_cast<D>(rhs.q)
            - static_cast<D>(this->q) * static_cast<D>(rhs.p);
    auto qd = static_cast<D>(this->q) * static_cast<D>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto& operator-=(const T& rhs) {
    auto pd = static_cast<D>(this->p)
            - static_cast<D>(this->q) * static_cast<D>(rhs);
    auto qd = static_cast<D>(this->q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto operator-(const rational<T, D, mode>& rhs) const {
    auto lhs = *this;
    return lhs -= rhs;
  }

  auto& operator*=(const rational<T, D, mode>& rhs) {
    auto pd = static_cast<D>(this->p) * static_cast<D>(rhs.p);
    auto qd = static_cast<D>(this->q) * static_cast<D>(rhs.q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto& operator*=(const T& rhs) {
    auto pd = static_cast<D>(this->p) * static_cast<D>(rhs);
    auto qd = static_cast<D>(this->q);

    normalize(pd, qd);

    this->p = static_cast<T>(pd);
    this->q = static_cast<T>(qd);

    return *this;
  }

  auto operator*(const rational<T, D, mode>& rhs) const {
    auto lhs = *this;
    return lhs *= rhs;
  }

  auto& operator/=(const rational<T, D, mode>& rhs) {
    return *this *= (1 / rhs);
  }

  auto& operator/=(const T& rhs) {
    return *this * (1 / rational<T, D, mode>{rhs});
  }

  auto operator/(const rational<T, D, mode>& rhs) const {
    auto lhs = *this;
    return lhs /= rhs;
  }

  auto operator==(const rational<T, D, mode>& rhs) const {
    if (mode == rtl::rational_mode::best) {
      return (this->p == rhs.p) && (this->q == rhs.q);
    } else {
      auto lhs_d = static_cast<D>(this->p) * static_cast<D>(rhs.q);
      auto rhs_d = static_cast<D>(rhs.p) * static_cast<D>(this->q);

      return lhs_d == rhs_d;
    }
  }

  auto operator!=(const rational<T, D, mode>& rhs) const {
    return !(*this == rhs);
  }
  
  auto operator<(const rational<T, D, mode>& rhs) const {
    auto lhs_d = static_cast<D>(this->p) * static_cast<D>(rhs.q);
    auto rhs_d = static_cast<D>(rhs.p) * static_cast<D>(this->q);

    return lhs_d < rhs_d;
  }

  auto operator>(const rational<T, D, mode>& rhs) const {
    return rhs < *this;
  }
  
  auto operator<=(const rational<T, D, mode>& rhs) const {
    return !(*this > rhs);
  }
  
  auto operator>=(const rational<T, D, mode>& rhs) const {
    return !(*this < rhs);
  }

private:
  constexpr static auto gcd(D a, D b) {
    auto gcd = D{1};

    while (b != 0) {
      gcd = b;
      b = a % b;
      a = gcd;
    }

    return gcd;
  }

  constexpr static auto normalize(D& pd, D& qd) {
    if (mode == rtl::rational_mode::best) {
      while (!(pd & 0b1) && !(qd & 0b1)) {
        pd >>= 1;
        qd >>= 1;
      }
  
      auto inv = modinv<D>(gcd(pd, qd));

      if (inv > 1) {
        pd *= inv;
        qd *= inv;
      }
    }

    while ((pd | qd) & ~std::numeric_limits<T>::max()) {
      pd >>= 1;
      qd >>= 1;
    }
  }

  T p;
  T q;
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
