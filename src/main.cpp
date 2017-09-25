#define RTL_CORTEX_M0

#include <rtl/platform.hpp>

#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/hdr_syscon.h>

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>
#include <hal/lpc1100/interrupt.hpp>
#include <hal/lpc1100/clock.hpp>

#include <sys/debug.hpp>

static void flash_access_time(uint32_t frequency);

static void flash_access_time(uint32_t frequency)
{
    uint32_t access_time, flashcfg_register;

    if (frequency < 20000000ul)             // 1 system clock for core speed below 20MHz
        access_time = FLASHCFG_FLASHTIM_1CLK;
    else if (frequency < 40000000ul)        // 2 system clocks for core speed between 20MHz and 40MHz
        access_time = FLASHCFG_FLASHTIM_2CLK;
    else                                    // 3 system clocks for core speed over 40MHz
        access_time = FLASHCFG_FLASHTIM_3CLK;

    // do not modify reserved bits in FLASHCFG register
    flashcfg_register = FLASHCFG;           // read register
    flashcfg_register &= ~(FLASHCFG_FLASHTIM_mask << FLASHCFG_FLASHTIM_bit);    // mask the FLASHTIM field
    flashcfg_register |= access_time << FLASHCFG_FLASHTIM_bit;  // use new FLASHTIM value
    FLASHCFG = flashcfg_register;           // save the new value back to the register
}

static void system_init(void)
{
    //LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;   // enable clock for IO configuration block
    // ^ shouldn't be needed anymore
    // LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6); // enable GPIO clock
    // ^ shouldn't be needed anymore
}

namespace dev = hal::lpc1100;

using assert_pin = dev::digital_output<dev::pin::PIO1_5>;
using output_pin = dev::digital_output<dev::pin::PIO0_8>;
using input_pin = dev::digital_input<dev::pin::PIO0_7>;

void assert_signal() {
  auto pin = assert_pin(hal::logic_level::high);

  for (auto i = 0; i < 5; ++i) {
    for (auto j = 0; j < 0x1FFFF; ++j) {
      pin.drive_high();
    }

    for (auto j = 0; j < 0x1FFFF; ++j) {
      pin.drive_low();
    }
  }
}

auto read_until(char* buffer, std::size_t& length, rtl::u8 pattern) {
  return [buffer, &length, pattern](const rtl::u8& data) {
    buffer[length++] = data;
    return data == pattern;
  };
}

auto read_any() {
  return [](const rtl::u8&) {
    return true;
  };
}

[[noreturn]] void main(const dev::reset_context& context) {
  system_init();

  auto frequency = 48000000;
  flash_access_time(frequency);           // configure flash access time first

  // set the main clock
  dev::clock<dev::clock_source::pll_in>::set_source(dev::clock_source::irc);
  dev::clock<dev::clock_source::pll_out>::enable(frequency);
  dev::clock<dev::clock_source::main>::set_source(dev::clock_source::pll_out);

  LPC_SYSCON->SYSAHBCLKDIV = 1;           // set AHB clock divider to 1

  auto uart = dev::uart0(9600);

  if (context.event == dev::reset_event::assert) {
    assert_signal();
  }

  auto output = output_pin(hal::logic_level::high);
  auto input = input_pin(input_pin::termination::pullup);

  //constexpr auto x = rtl::q32<rtl::rational_mode::best>(1.666981f);
  constexpr auto x = rtl::r32(18481.38438f);
  constexpr auto y = rtl::r32(3.1415926535897932384626433832795);

  uart.read(read_any()).wait();

  constexpr auto foo = rtl::MHz<rtl::q32>{{100.0f}};
  constexpr auto bar = rtl::Hz<rtl::q32>{foo};

  /*
  sys::debug(uart, std::pair{"%10s", bar.value().numerator()});
  sys::debug(uart, std::pair{"", " / "});
  sys::debug(uart, std::pair{"%10s", bar.value().denominator()});
  sys::debug(uart, std::pair{"", "\r\n"});

  sys::debug(uart,
     std::pair{"%10s", "FRAC = "},
     std::pair{"%10s", y.numerator()},
     std::pair{"%10s", " / "},
     std::pair{"%10s", y.denominator()},
     std::pair{"%10s", "\r\n"});
  */

  uart.write(sys::debug(std::pair{"10s", "HELLO WORLD!!!"}, std::pair{"", "some text"})).wait();

  //output.drive_low();

  rtl::assert<x + (y - x) - 2.5f <= x + x>("test");

  //output.drive_high();  

  #if 0
  const char map[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char buf[8];

  auto x = uart.read({buf, 1});
  x.wait();

  int test;

  for (auto i = 0; i < 8; ++i) {
      buf[7 - i] = map[((0x10000400 - ((rtl::uptr)&test)) >> (4 * i)) & 0xF];
  }

  uart.write({buf, sizeof(buf)}).wait();
  #endif

  //auto gpio2 = dev::gpio2<dev::pin::PIO0_8>(/* termination, interrupt settings, etc.. */);
  //auto uart = dev::uart<dev::pin::PIO1_7, dev::pin::PIO1_6>(/* uart settings */);

  /*
  while (true) {
    char buffer[32];
    std::size_t pos = 0;
    std::size_t offset = 0;

    uart.read(read_until(buffer, pos, '\r')).wait();

    uart.write([&](rtl::u8& data) {
      if (offset == pos) {
          return true;
      }

      data = buffer[offset++];
      return false;
    }).wait();
  }
  */

  /*
  while (true) {
    uart.send("hello\r\n", 7);

    for (auto i = 0; i < 0xFFFF; ++i) { __NOP(); };
  }
  */

  while (true) {
    for (auto i = 0; i < 0x1FFFFF; ++i) {
      output.drive_low();
    }

    for (auto i = 0; i < 0x1FFFFF; ++i) {
      output.drive_high();
    }
  }
}
