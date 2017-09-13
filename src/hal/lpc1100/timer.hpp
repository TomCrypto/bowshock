#pragma once

/// @file
///
/// @brief Timer for LPC1100 series microcontrollers.

#include <rtl/base.hpp>

namespace hal::lpc1100 {

// this version does "timer" functionality - count clock ticks, and on a specific number of matches, perform actions such
// as 'reset', 'stop', 'reset'. The templated timers take up to 4 "external match" pins, which can be driven low,
// high, or toggled on a match on the corresponding match register.

// TODO: for the external match pins, they can have an "index" constexpr int on them specifying which ones they are (so
// that you could do timer32<EM0, EM1, EM2> or timer32<EM3, EM1> if you wanted?)

// In terms of resource ownership, each timer can be owned by one owner at any given time. The templated variants exist
// only to allow those unused match pins to be used for other purposes (e.g. GPIO).

template <typename N> class timer32 {
public:
  // TODO

private:
  // TODO

};

}
