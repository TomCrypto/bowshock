#pragma once

/// @file
///
/// Fundamental definitions needed by the RTL.
///
/// @remarks This should be the only header which includes system headers.

/// @cond
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <limits>
#include <new>
/// @endcond

extern "C" void abort();
extern "C" void* memcpy(void* destination, const void* source, std::size_t num);
extern "C" int memcmp(const void* a, const void* b, std::size_t num);

namespace rtl {

using uptr = uintptr_t;
using iptr = intptr_t;
using u64  = uint64_t;
using i64  = uint64_t;
using u32  = uint32_t;
using u16  = uint16_t;
using u8   = uint8_t;
using i32  = int32_t;
using i16  = int16_t;
using i8   = int8_t;

static_assert(std::numeric_limits<u8>::digits == 8, "unsupported integral type");
static_assert(std::numeric_limits<u16>::digits == 16, "unsupported integral type");
static_assert(std::numeric_limits<u32>::digits == 32, "unsupported integral type");
static_assert(std::numeric_limits<u64>::digits == 64, "unsupported integral type");

class noncopyable {
protected:
  noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(noncopyable&& other) = default;
  noncopyable& operator=(noncopyable&&) = default;
private:
  noncopyable(const noncopyable& other) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

class nonmoveable : private noncopyable {
private:
  nonmoveable(nonmoveable&& other) = delete;
  nonmoveable& operator=(nonmoveable&&) = delete;
};

}
