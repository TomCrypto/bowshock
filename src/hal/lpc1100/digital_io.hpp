#pragma once

/// @file
///
/// @brief Digital IO for the LPC1100 series microcontrollers.
///
/// @remarks Because the I2C pins (PIO0_4/SCL and PIO0_5/SDA) have built-in non-programmable open-drain circuitry, it
///          is not possible to use these physical pins as digital outputs without an external pull-up resistor. They
///          however remain configurable as digital inputs.

#include <rtl/mmio.hpp>

#include <hal/digital_io.hpp>
#include <hal/lpc1100/physical_io.hpp>

namespace hal::lpc1100 {

  template <pin pin> class digital_input;
  template <pin pin> class digital_output;

namespace {

template <pin pin, rtl::uptr gpio_ptr, std::size_t port_no>
class basic_digital_input : public hal::digital_input<basic_digital_input<pin, gpio_ptr, port_no>> {
private:
  using physical_io_t = typename hal::lpc1100::physical_io<pin>;
  physical_io_t physical_io;

public:
  using termination = typename physical_io_t::termination;
  using options = typename physical_io_t::digital_input_options;

  basic_digital_input(termination termination, options options = options::none)
    : physical_io(physical_io_t::function::gpio, termination, options) {
    gpio_dir.template clear<port_mask>();
  }

  auto state() {
    return gpio_data.read() ? hal::logic_level::high : hal::logic_level::low;
  }

private:
  static constexpr auto port_mask = 1 << port_no;

  rtl::mmio<rtl::u32> gpio_data{gpio_ptr + 4 * port_mask};
  rtl::mmio<rtl::u32> gpio_dir{gpio_ptr + 0x8000};
};

template <pin pin, rtl::uptr gpio_ptr, std::size_t port_no>
class basic_digital_output : public hal::digital_output<basic_digital_output<pin, gpio_ptr, port_no>> {
private:
  using physical_io_t = typename hal::lpc1100::physical_io<pin>;
  physical_io_t physical_io;

public:
  using options = typename physical_io_t::digital_output_options;

  basic_digital_output(hal::logic_level initial_level, options options = options::none)
    : physical_io(physical_io_t::function::gpio, options) {
    this->drive(initial_level);
    gpio_dir.template set<port_mask>();
  }

  void drive_low() {
    gpio_data.clear();
  }

  void drive_high() {
    gpio_data.set();
  }

private:
  static constexpr auto port_mask = 1 << port_no;

  rtl::mmio<rtl::u32> gpio_data{gpio_ptr + 4 * port_mask};
  rtl::mmio<rtl::u32> gpio_dir{gpio_ptr + 0x8000};
};

}

#define DIGITAL_INPUT(PIN, GPIO, PORT_NO) \
  template <> class digital_input<PIN> : public basic_digital_input<PIN, GPIO, PORT_NO> { using basic_digital_input::basic_digital_input; };

#define DIGITAL_OUTPUT(PIN, GPIO, PORT_NO) \
template <> class digital_output<PIN> : public basic_digital_output<PIN, GPIO, PORT_NO> { using basic_digital_output::basic_digital_output; };

#define DIGITAL_IO(PIN, GPIO, PORT_NO) \
  DIGITAL_INPUT(PIN, GPIO, PORT_NO) \
  DIGITAL_OUTPUT(PIN, GPIO, PORT_NO)

DIGITAL_IO(pin::PIO0_0,  0x50000000,  0)
DIGITAL_IO(pin::PIO0_1,  0x50000000,  1)
DIGITAL_IO(pin::PIO0_2,  0x50000000,  2)
DIGITAL_IO(pin::PIO0_3,  0x50000000,  3)
DIGITAL_INPUT(pin::PIO0_4, 0x50000000, 4)
DIGITAL_INPUT(pin::PIO0_5, 0x50000000, 5)
DIGITAL_IO(pin::PIO0_6,  0x50000000,  6)
DIGITAL_IO(pin::PIO0_7,  0x50000000,  7)
DIGITAL_IO(pin::PIO0_8,  0x50000000,  8)
DIGITAL_IO(pin::PIO0_9,  0x50000000,  9)
DIGITAL_IO(pin::PIO0_10, 0x50000000, 10)
DIGITAL_IO(pin::PIO0_11, 0x50000000, 11)
DIGITAL_IO(pin::PIO1_0,  0x50010000,  0)
DIGITAL_IO(pin::PIO1_1,  0x50010000,  1)
DIGITAL_IO(pin::PIO1_2,  0x50010000,  2)
DIGITAL_IO(pin::PIO1_3,  0x50010000,  3)
DIGITAL_IO(pin::PIO1_4,  0x50010000,  4)
DIGITAL_IO(pin::PIO1_5,  0x50010000,  5)
DIGITAL_IO(pin::PIO1_6,  0x50010000,  6)
DIGITAL_IO(pin::PIO1_7,  0x50010000,  7)
DIGITAL_IO(pin::PIO1_8,  0x50010000,  8)
DIGITAL_IO(pin::PIO1_9,  0x50010000,  9)
DIGITAL_IO(pin::PIO1_10, 0x50010000, 10)
DIGITAL_IO(pin::PIO1_11, 0x50010000, 11)
DIGITAL_IO(pin::PIO2_0,  0x50020000,  0)
DIGITAL_IO(pin::PIO2_1,  0x50020000,  1)
DIGITAL_IO(pin::PIO2_2,  0x50020000,  2)
DIGITAL_IO(pin::PIO2_3,  0x50020000,  3)
DIGITAL_IO(pin::PIO2_4,  0x50020000,  4)
DIGITAL_IO(pin::PIO2_5,  0x50020000,  5)
DIGITAL_IO(pin::PIO2_6,  0x50020000,  6)
DIGITAL_IO(pin::PIO2_7,  0x50020000,  7)
DIGITAL_IO(pin::PIO2_8,  0x50020000,  8)
DIGITAL_IO(pin::PIO2_9,  0x50020000,  9)
DIGITAL_IO(pin::PIO2_10, 0x50020000, 10)
DIGITAL_IO(pin::PIO2_11, 0x50020000, 11)
DIGITAL_IO(pin::PIO3_0,  0x50030000,  0)
DIGITAL_IO(pin::PIO3_1,  0x50030000,  1)
DIGITAL_IO(pin::PIO3_2,  0x50030000,  2)
DIGITAL_IO(pin::PIO3_3,  0x50030000,  3)
DIGITAL_IO(pin::PIO3_4,  0x50030000,  4)
DIGITAL_IO(pin::PIO3_5,  0x50030000,  5)
#undef DIGITAL_IO
#undef DIGITAL_OUTPUT
#undef DIGITAL_INPUT

}
