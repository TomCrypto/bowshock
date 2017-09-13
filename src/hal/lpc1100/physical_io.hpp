#pragma once

/// @file
///
/// @brief Physical IO pin definitions for the LPC1100 series microcontrollers.

#include <rtl/base.hpp>
#include <rtl/mmio.hpp>
#include <rtl/bitflags.hpp>

// TODO: LPC_SYSCON->SYSAHBCLKCTRL
// this is the register where we need to enable the IOCON clock to make any of these changes
// make sure to enable it here? (and disable it after the writes)
// maybe we need a free function here that wraps an IOCON "operation" - it could take a lambda?

namespace hal::lpc1100 {

enum class pin {
  PIO0_0   =   0, RESET = 0,
  PIO0_1   =   1, CLKOUT = 1, CT32B0_MAT2 = 2,
  PIO0_2   =   2, SSEL0 = 2, CT16B0_CAP0 = 2,
  PIO0_3   =   3,
  PIO0_4   =   4, SCL = 4,
  PIO0_5   =   5, SDA = 5,
  PIO0_6   =   6, SCK0_A = 6,
  PIO0_7   =   7, CTS = 7,
  PIO0_8   =   8, MISO0 = 8, CT16B0_MAT0 = 8,
  PIO0_9   =   9, MOSI0 = 9, CT16B0_MAT1 = 9,
  PIO0_10  =  10, SWCLK = 10, SCK0_B = 10, CT16B0_MAT2 = 10,
  PIO0_11  =  11, AD0 = 11, CT32B0_MAT3 = 11,
  PIO1_0   =  12, AD1 = 12, CT32B1_CAP0 = 12,
  PIO1_1   =  13, AD2 = 13, CT32B1_MAT0 = 13,
  PIO1_2   =  14, AD3 = 14, CT32B1_MAT1 = 14,
  PIO1_3   =  15, SWDIO = 15, AD4 = 15, CT32B1_MAT2 = 15,
  PIO1_4   =  16, AD5 = 16, CT32B1_MAT3 = 16, WAKEUP = 16,
  PIO1_5   =  17, RTS = 17, CT32B0_CAP0 = 17,
  PIO1_6   =  18, RXD = 18, CT32B0_MAT0 = 18,
  PIO1_7   =  19, TXD = 19, CT32B0_MAT1 = 19,
  PIO1_8   =  20, CT16B1_CAP0 = 20,
  PIO1_9   =  21, CT16B1_MAT0 = 21,
  PIO1_10  =  22, AD6 = 22, CT16B1_MAT1 = 22,
  PIO1_11  =  23, AD7 = 23,
  PIO2_0   =  24, DTR_A = 24, SSEL1 = 24,
  PIO2_1   =  25, DSR_A = 25, SCK1 = 25,
  PIO2_2   =  26, DCD_A = 26, MISO1 = 26,
  PIO2_3   =  27, RI_A = 27, MOSI1 = 27,
  PIO2_4   =  28,
  PIO2_5   =  29,
  PIO2_6   =  30,
  PIO2_7   =  31,
  PIO2_8   =  32,
  PIO2_9   =  33,
  PIO2_10  =  34,
  PIO2_11  =  35, SCK0_C = 35,
  PIO3_0   =  36, DTR_B = 36,
  PIO3_1   =  37, DSR_B = 37,
  PIO3_2   =  38, DCD_B = 38,
  PIO3_3   =  39, RI_B = 39,
  PIO3_4   =  40,
  PIO3_5   =  41,
  none
};

template <pin pin> class physical_io;

namespace {
  enum class basic_termination : rtl::u32 {
    none     = 0b00 << 3,
    pulldown = 0b01 << 3,
    pullup   = 0b10 << 3,
    repeater = 0b11 << 3
  };

  enum class basic_digital_input_options : rtl::u32 {
    none = 0,
    hysteresis = 0b1 << 5
  };

  enum class basic_digital_output_options : rtl::u32 {
    none = 0
  };
};

namespace detail {
  inline auto SYSAHBCLKCTRL() { return rtl::mmio<rtl::u32>{0x40048080}; }

  struct iocon_mmio : public rtl::mmio<rtl::u32> {
    iocon_mmio(rtl::uptr addr) : rtl::mmio<rtl::u32>(addr) {
      SYSAHBCLKCTRL().template set_bit<16>();
    }

    ~iocon_mmio() {
      SYSAHBCLKCTRL().template clear_bit<16>();
    }
  };
};

template <> class physical_io<pin::PIO0_0> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004400C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_1> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044010}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_2> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004401C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_3> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004402C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_4> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044030}; }
public:
  enum class termination : rtl::u32 {
    none = 0
  };

  using digital_input_options = basic_digital_input_options;
  
  physical_io(termination /* termination */, digital_input_options options) {
    IOCON().template write<0b1100000111>(0b000 | static_cast<rtl::u32>(options) | (0b01 << 8));
  }
};

template <> class physical_io<pin::PIO0_5> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044034}; }
public:
  enum class termination : rtl::u32 {
    none = 0
  };

  using digital_input_options = basic_digital_input_options;
  
  physical_io(termination /* termination */, digital_input_options options) {
    IOCON().template write<0b1100000111>(0b000 | static_cast<rtl::u32>(options) | (0b01 << 8));
  }
};

template <> class physical_io<pin::PIO0_6> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004404C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_7> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044050}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_8> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044060}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_9> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044064}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_10> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044068}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO0_11> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044074}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 |static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_0> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044078}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_1> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004407C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_2> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044080}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_3> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044090}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_4> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044094}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_5> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x400440A0}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_6> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x400440A4}; }
public:
  enum class uart_rx_options {
    none = 0
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }

  physical_io(uart_rx_options /*options*/) {
    IOCON().template write<0b111>(0b001);
  }
};

template <> class physical_io<pin::PIO1_7> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x400440A8}; }
public:
  enum class uart_tx_options {
    none = 0
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }

  physical_io(uart_tx_options /*options*/) {
    IOCON().template write<0b111>(0b001);
  }
};

template <> class physical_io<pin::PIO1_8> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044014}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_9> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044038}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_10> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004406C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b001 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b001 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO1_11> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044098}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_0> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044008}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(
      0b000 |
      static_cast<rtl::u32>(options)
    );
  }
};

template <> class physical_io<pin::PIO2_1> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044028}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_2> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004405C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_3> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004408C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_4> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044040}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_5> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044044}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_6> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044000}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_7> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044020}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_8> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044024}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_9> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044054}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_10> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044058}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO2_11> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044070}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_0> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044084}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_1> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044088}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_2> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004409C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_3> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x400440AC}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_4> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x4004403C}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

template <> class physical_io<pin::PIO3_5> {
private:
  static auto IOCON() { return detail::iocon_mmio{0x40044048}; }
public:
  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  
  physical_io(termination termination, digital_input_options options) {
    IOCON().template write<0b111111>(0b000 | static_cast<rtl::u32>(termination) | static_cast<rtl::u32>(options));
  }

  physical_io(digital_output_options options) {
    IOCON().template write<0b111>(0b000 | static_cast<rtl::u32>(options));
  }
};

}

namespace rtl {

// TODO: define any bitflags for options and stuff
// template <> struct is_bitflags<hal::lpc1100::non_i2c_options> : std::true_type {};

}
