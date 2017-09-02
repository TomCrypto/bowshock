#pragma once

/// @file
///
/// @brief Mathematical unit types.

#include <rtl/base.hpp>

// TODO: this needs LOTS of improvements to be feature-complete
// but is also the easiest to implement!

namespace rtl {

struct frequency {
public:
  frequency(unsigned int hertz) : hertz(hertz) {}

  auto ticks_per_second() const {
    return hertz;
  }

  auto nanoseconds_per_tick() const {
    return 1000000000 / hertz;
  }

  static auto none() {
    return frequency{0};
  }

  frequency operator*(const frequency& other) {
    return frequency{hertz * other.hertz};
  }

private:
  unsigned int hertz;
};

}
