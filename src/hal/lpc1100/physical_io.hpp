#pragma once

/// @file
///
/// @brief Physical IO pin definitions for the LPC1100 series microcontrollers.

#include <rtl/base.hpp>
#include <rtl/bitflags.hpp>

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

template <> class physical_io<pin::PIO0_0> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004400C;
};

template <> class physical_io<pin::PIO0_1> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044010;
};

template <> class physical_io<pin::PIO0_2> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004401C;
};

template <> class physical_io<pin::PIO0_3> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004402C;
};

template <> class physical_io<pin::PIO0_4> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  enum class termination : rtl::u32 {
    none = 0
  };

  using digital_input_options = basic_digital_input_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination /* termination */, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b1100000111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options) |
      (0b01 << 8)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044030;
};

template <> class physical_io<pin::PIO0_5> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  enum class termination : rtl::u32 {
    none = 0
  };

  using digital_input_options = basic_digital_input_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination /* termination */, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b1100000111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options) |
      (0b01 << 8)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044034;
};

template <> class physical_io<pin::PIO0_6> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004404C;
};

template <> class physical_io<pin::PIO0_7> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044050;
};

template <> class physical_io<pin::PIO0_8> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044060;
};

template <> class physical_io<pin::PIO0_9> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044064;
};

template <> class physical_io<pin::PIO0_10> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044068;
};

template <> class physical_io<pin::PIO0_11> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044074;
};

template <> class physical_io<pin::PIO1_0> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044078;
};

template <> class physical_io<pin::PIO1_1> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004407C;
};

template <> class physical_io<pin::PIO1_2> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044080;
};

template <> class physical_io<pin::PIO1_3> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044090;
};

template <> class physical_io<pin::PIO1_4> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044094;
};

template <> class physical_io<pin::PIO1_5> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x400440A0;
};

template <> class physical_io<pin::PIO1_6> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x400440A4;
};

template <> class physical_io<pin::PIO1_7> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x400440A8;
};

template <> class physical_io<pin::PIO1_8> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044014;
};

template <> class physical_io<pin::PIO1_9> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044038;
};

template <> class physical_io<pin::PIO1_10> {
public:
  enum class function : rtl::u32 {
    gpio = 0b001
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004406C;
};

template <> class physical_io<pin::PIO1_11> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044098;
};

template <> class physical_io<pin::PIO2_0> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044008;
};

template <> class physical_io<pin::PIO2_1> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044028;
};

template <> class physical_io<pin::PIO2_2> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004405C;
};

template <> class physical_io<pin::PIO2_3> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004408C;
};

template <> class physical_io<pin::PIO2_4> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044040;
};

template <> class physical_io<pin::PIO2_5> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044044;
};

template <> class physical_io<pin::PIO2_6> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044000;
};

template <> class physical_io<pin::PIO2_7> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044020;
};

template <> class physical_io<pin::PIO2_8> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044024;
};

template <> class physical_io<pin::PIO2_9> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044054;
};

template <> class physical_io<pin::PIO2_10> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044058;
};

template <> class physical_io<pin::PIO2_11> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044070;
};

template <> class physical_io<pin::PIO3_0> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044084;
};

template <> class physical_io<pin::PIO3_1> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044088;
};

template <> class physical_io<pin::PIO3_2> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004409C;
};

template <> class physical_io<pin::PIO3_3> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x400440AC;
};

template <> class physical_io<pin::PIO3_4> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x4004403C;
};

template <> class physical_io<pin::PIO3_5> {
public:
  enum class function : rtl::u32 {
    gpio = 0b000
  };

  using termination = basic_termination;
  using digital_input_options = basic_digital_input_options;
  using digital_output_options = basic_digital_output_options;
  // options for other functions (e.g. open drive options) can be added here

  physical_io(function function, termination termination, digital_input_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(termination) |
      static_cast<rtl::u32>(options)
    );
  }

  physical_io(function function, digital_output_options options) {
    auto iocon = rtl::mmio<rtl::u32>(iocon_ptr);

    iocon.template write<0b111>(
      static_cast<rtl::u32>(function) |
      static_cast<rtl::u32>(options)
    );
  }

private:
  static constexpr rtl::u32 iocon_ptr = 0x40044048;
};

}

namespace rtl {

// TODO: define any bitflags for options and stuff
// template <> struct is_bitflags<hal::lpc1100::non_i2c_options> : std::true_type {};

}
