#pragma once

/// @file
///
/// @brief Clock control for the LPC1100 series microcontrollers.

#include <rtl/base.hpp>
#include <rtl/mmio.hpp>
#include <rtl/assert.hpp>
#include <rtl/units.hpp>

namespace hal::lpc1100 {

/// @brief All clock sources available in the system.
enum class clock_source {
  irc,            ///< Internal IRC oscillator
  system,         ///< External system crystal oscillator
  pll_in,         ///< On-chip phase-locked loop input
  pll_out,        ///< On-chip phase-locked loop output
  watchdog_osc,   ///< Watchdog oscillator
  watchdog,       ///< Watchdog clock
  main,           ///< Main system-wide clock
  core,           ///< Core clock (AHB)
  clkout,         ///< CLKOUT source clock
  uart            ///< UART peripheral clock
};

template <clock_source source> class clock;

/// @brief The internal IRC oscillator clock.
template <> class clock<clock_source::irc> {
private:
  static auto PDRUNCFG() { return rtl::mmio<rtl::u32>{0x40048238}; }

public:
  template <typename T> static auto frequency() { return rtl::quantity<T, rtl::megahertz>{12}; }

  static auto disable() {
    PDRUNCFG().set<0b11>();
  }

  static auto enable() {
    PDRUNCFG().clear<0b11>();
  }
};

// TODO: implement system clock, and that will be enough for now

template <> class clock<clock_source::pll_in> {
public:
  template <typename T> static auto frequency() {
    auto SYSPLLCLKSEL = rtl::mmio<rtl::u32>{0x40048040};

    switch (SYSPLLCLKSEL.read<0b1>()) {
      case 0b0:
        return clock<clock_source::irc>::frequency<T>();
      case 0b1:
        //return clock<clock_source::system>::frequency();
      default:
        rtl::unreachable(TRACE("invalid clock configuration"));
    }
  }

  static auto set_source(clock_source source) {
    auto SYSPLLCLKSEL = rtl::mmio<rtl::u32>{0x40048040};
    auto SYSPLLCLKUEN = rtl::mmio<rtl::u32>{0x40048044};

    switch (source) {
      case clock_source::irc:
        SYSPLLCLKSEL.write<0b1>(0b0);
        break;
      case clock_source::system:
        SYSPLLCLKSEL.write<0b1>(0b1);
        break;
      default:
        rtl::assert(false, TRACE("invalid clock source provided"));
    }

    SYSPLLCLKUEN.write<0b1>(0b0);
    SYSPLLCLKUEN.write<0b1>(0b1);
  }
};

template <> class clock<clock_source::pll_out> {
public:
  template <typename T> static auto frequency() {
    auto SYSPLLCTRL = rtl::mmio<rtl::u32>{0x40048008};

    auto m = SYSPLLCTRL.read<0b11111>() + 1;

    return m * clock<clock_source::pll_in>::frequency<T>();
  }

  template <typename T> static auto enable(T frequency) {
    // TODO: configure PLL accordingly somehow (this is probably incomplete)

    auto frequency_in = clock<clock_source::pll_in>::frequency<rtl::u32>();

    // calculate PLL parameters
    auto m = frequency / frequency_in;                // M is the PLL multiplier
    auto fcco = frequency_in * 2 * m;                 // FCCO is the internal PLL frequency

    auto p = 0;

    while (fcco < 156_MHz) {
      fcco = fcco * 2;
      p++;                                // find P which gives FCCO in the allowed range (over 156MHz)
    }

    auto SYSPLLCTRL = rtl::mmio<rtl::u32>{0x40048008};
    auto SYSPLLSTAT = rtl::mmio<rtl::u32>{0x4004800C};
    auto PDRUNCFG = rtl::mmio<rtl::u32>{0x40048238};

    SYSPLLCTRL.write<0b111111>((m.template as<rtl::dimensionless>() - 1) | p << 5);
    PDRUNCFG.clear<0b10000000>(); // power-up PLL

    while (SYSPLLSTAT.none<0b1>());    // wait for PLL lock
  }

  static auto disable() {
    auto PDRUNCFG = rtl::mmio<rtl::u32>{0x40048238};
    PDRUNCFG.set<0b10000000>(); // power-down PLL
  }
};

/// @brief The main system-wide clock.
///
/// @remarks This clock cannot be powered down while the system is running.
template <> class clock<clock_source::main> {
public:
  template <typename T> static auto frequency() {
    auto MAINCLKSEL = rtl::mmio<rtl::u32>{0x40048070};

    switch (MAINCLKSEL.read<0b11>()) {
      case 0b00:
        return clock<clock_source::irc>::frequency<T>();
      case 0b01:
        return clock<clock_source::pll_in>::frequency<T>();
      case 0b10:
        //return clock<clock_source::watchdog_osc>::frequency();
      case 0b11:
        return clock<clock_source::pll_out>::frequency<T>();
      default:
        rtl::unreachable(TRACE("invalid clock configuration"));
    }
  }

  static auto set_source(clock_source source) {
    auto MAINCLKSEL = rtl::mmio<rtl::u32>{0x40048070};
    auto MAINCLKUEN = rtl::mmio<rtl::u32>{0x40048074};

    switch (source) {
      case clock_source::irc:
        MAINCLKSEL.write<0b11>(0b00);
        break;
      case clock_source::pll_in:
        MAINCLKSEL.write<0b11>(0b01);
        break;
      case clock_source::pll_out:
        MAINCLKSEL.write<0b11>(0b11);
        break;
      case clock_source::watchdog_osc:
        MAINCLKSEL.write<0b11>(0b10);
        break;
      default:
        rtl::assert(false, TRACE("invalid clock source provided"));
    }

    MAINCLKUEN.write<0b1>(0b0);
    MAINCLKUEN.write<0b1>(0b1);
  }
};

/// @brief The UART peripheral clock.
template <> class clock<clock_source::uart> {
private:
  static auto SYSAHBCLKCTRL() { return rtl::mmio<rtl::u32>{0x40048080}; }
  static auto UARTCLKDIV() { return rtl::mmio<rtl::u32>{0x40048098}; }

public:
  template <typename T> static auto frequency() {
    auto divider = UARTCLKDIV().read<0b11111111>();
    return clock<clock_source::main>::frequency<T>() / divider;
  }

  static auto set_divider(rtl::u8 divider) {
    rtl::assert(divider != 0, TRACE("UART clock enabled with zero divider"));
    UARTCLKDIV().write<0b11111111>(divider);
  }

  static auto enable(rtl::u8 divider = 1) {
    set_divider(divider);
    SYSAHBCLKCTRL().set_bit<12>();
  }

  static auto disable() {
    SYSAHBCLKCTRL().clear_bit<12>();
  }
};

}
