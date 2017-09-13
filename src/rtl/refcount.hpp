#pragma once

/// @file
///
/// @brief Reference counting utilities.

namespace rtl {

template <typename Acquire, typename Release> struct resource {
  void acquire() {
    if (refcount++ == 0) {
      Acquire();
    }
  }

  void release() {
    if (--refcount == 0) {
      Release();
    }
  }

private:
  int refcount;
};

}
