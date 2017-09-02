#include <rtl/platform.hpp>

#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/hdr_syscon.h>

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>
#include <hal/lpc1100/interrupt.hpp>

enum ClockSource {
  ExternalCrystal,
  InternalOscillator
};

static void flash_access_time(uint32_t frequency);
static uint32_t pll_start(enum ClockSource source, uint32_t crystal, uint32_t frequency);

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

static uint32_t pll_start(enum ClockSource source, uint32_t crystal, uint32_t frequency)
{
    uint32_t m, p = 0, fcco;

    flash_access_time(frequency);           // configure flash access time first

    // SYSOSCCTRL_FREQRANGE should be 0 for crystals in range 1 - 20MHz
    // SYSOSCCTRL_FREQRANGE should be 1 for crystals in range 15 - 25MHz
    if (crystal < 17500000)                 // divide the ranges on 17.5MHz then
        LPC_SYSCON->SYSOSCCTRL = 0;         // "lower speed" crystals
    else
        LPC_SYSCON->SYSOSCCTRL = SYSOSCCTRL_FREQRANGE;  // "higher speed" crystals

    LPC_SYSCON->PDRUNCFG &= ~PDRUNCFG_SYSOSC_PD;    // power-up main oscillator

    if (source == ExternalCrystal) {
        LPC_SYSCON->SYSPLLCLKSEL = SYSPLLCLKSEL_SEL_SYSOSC;
    } else {
        LPC_SYSCON->SYSPLLCLKSEL = SYSPLLCLKSEL_SEL_IRC;
    }

    LPC_SYSCON->SYSPLLCLKUEN = 0;           // confirm the change of PLL input clock by toggling the...
    LPC_SYSCON->SYSPLLCLKUEN = SYSPLLUEN_ENA;   // ...ENA bit in LPC_SYSCON->SYSPLLCLKUEN register

    // calculate PLL parameters
    m = frequency / crystal;                // M is the PLL multiplier
    fcco = m * crystal * 2;                 // FCCO is the internal PLL frequency

    frequency = crystal * m;

    while (fcco < 156000000)
    {
        fcco *= 2;
        p++;                                // find P which gives FCCO in the allowed range (over 156MHz)
    }

    LPC_SYSCON->SYSPLLCTRL = ((m - 1) << SYSPLLCTRL_MSEL_bit) | (p << SYSPLLCTRL_PSEL_bit); // configure PLL
    LPC_SYSCON->PDRUNCFG &= ~PDRUNCFG_SYSPLL_PD; // power-up PLL

    while (!(LPC_SYSCON->SYSPLLSTAT & SYSPLLSTAT_LOCK));    // wait for PLL lock

    LPC_SYSCON->MAINCLKSEL = MAINCLKSEL_SEL_PLLOUT; // select PLL output as the main clock
    LPC_SYSCON->MAINCLKUEN = 0;             // confirm the change of main clock by toggling the...
    LPC_SYSCON->MAINCLKUEN = MAINCLKUEN_ENA;    // ...ENA bit in LPC_SYSCON->MAINCLKUEN register

    LPC_SYSCON->SYSAHBCLKDIV = 1;           // set AHB clock divider to 1

    return frequency;
}

static void system_init(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;   // enable clock for IO configuration block
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6); // enable GPIO clock
}

namespace dev = hal::lpc1100;

using assert_pin = dev::digital_output<dev::pin::PIO1_5>;
using output_pin = dev::digital_output<dev::pin::PIO0_8>;
using input_pin = dev::digital_input<dev::pin::PIO0_7>;

void assert_signal() {
  auto pin = assert_pin(hal::logic_level::high);

  for (auto i = 0; i < 5; ++i) {
    for (auto j = 0; j < 0xFFFF; ++j) {
      pin.drive_high();
    }

    for (auto j = 0; j < 0xFFFF; ++j) {
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

[[noreturn]] void main(const dev::reset_context& context) {
  system_init();

  /*
  // configure the PLL to generate a 48MHz clock
  hal::clock<hal::pll>::configure(hal::source::irc,
                                  48000000);
  // configure the main clock to use the PLL as a source
  hal::clock<hal::main>::set_source(hal::pll);
  // configure CLKOUT to use the PLL as a source
  hal::clock<hal::clkout>::set_source(hal::pll);
  // and give it a x4 divider to get a 12MHz output clock
  hal::clock<hal::clkout>::set_divider(4);
  // how to encapsulate the above nicely?


  auto pll = hal::clock<hal::pll>(hal::clock_source::irc,
                                  48000000);
  */

  pll_start(InternalOscillator, 12000000, 48000000);

  auto uart = dev::uart0(115200);

  if (context.event == dev::reset_event::assert) {
    std::size_t length = 0;

    while (true) {
        if (context.assert.message[length++] == '\0') {
            break;
        }
    }

    //uart.write({const_cast<char*>(context.assert.message), length - 1}).wait();

    assert_signal();
  }

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

  auto output = output_pin(hal::logic_level::high);
  auto input = input_pin(input_pin::termination::pullup);

  //auto gpio2 = dev::gpio2<dev::pin::PIO0_8>(/* termination, interrupt settings, etc.. */);
  //auto uart = dev::uart<dev::pin::PIO1_7, dev::pin::PIO1_6>(/* uart settings */);

  while (true) {
    char buffer[32];
    std::size_t pos = 0;
    std::size_t offset = 0;

    /*auto read_waitable = uart.read([&](const rtl::u8& data) {
      buffer[pos++] = data;
      return data == '\r';
    });*/
    auto read_waitable = uart.read(read_until(buffer, pos, '\r'));
    read_waitable.wait();

    uart.write([&](rtl::u8& data) {
      if (offset == pos) {
          return true;
      }

      data = buffer[offset++];
      return false;
    }).wait();
  }

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

    /*
    output.drive(input.state());

    rtl::assert(input.state() == hal::logic_level::high, "fuck");
    */
  }
}
