#define RTL_CORTEX_M0

#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/hdr_syscon.h>

#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/uart.hpp>

namespace dev = hal::lpc1100;

/*

syntax:

[peripheral] [command] [arguments...]

example:

digital_output init PIO0_1 low
digital_output drive PIO0_1 high

system sleep 1000

handlers could work like this:

void handler(iterator word, iterator end) {
  if (word == "PIO0_0") {
    gpio_handler<PIO0_0>(word + 1, end);
  } else if ...
}

*/

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

struct word {
  const char* ptr;
  std::size_t len;
};

dev::digital_output<dev::pin::PIO0_8> gpio{hal::logic_level::low};

void digital_output_drive_handler(const word* words, std::size_t count) {
  if (memcmp(words[0].ptr, "low", words[0].len) == 0) {
    gpio.drive_low();
  } else if (memcmp(words[0].ptr, "high", words[0].len) == 0) {
    gpio.drive_high();
  }
}

void digital_output_handler(const word* words, std::size_t count) {
  if (memcmp(words[0].ptr, "init", words[0].len) == 0) {
    //digital_output_init_handler(words + 1, count - 1);
  } else if (memcmp(words[0].ptr, "drive", words[0].len) == 0) {
    digital_output_drive_handler(words + 1, count - 1);
  }
}

void handler(const word* words, std::size_t count) {
  if (memcmp(words[0].ptr, "digital_output", words[0].len) == 0) {
    digital_output_handler(words + 1, count - 1);
  }
}

char buffer[1024];
word words[32];

[[noreturn]] void main(const dev::reset_context& context) {
  //LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_IOCON;   // enable clock for IO configuration block
  // ^ shouldn't be needed anymore
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6); // enable GPIO clock
  // ^ shouldn't be needed anymore

  auto frequency = 12000000;
  flash_access_time(frequency);           // configure flash access time first

  gpio = dev::digital_output<dev::pin::PIO0_8>(hal::logic_level::low);

  #if 0
  auto uart = dev::uart0(9600);

  while (true) {
    std::size_t word_index = 0;
    std::size_t data_index = 0;

    words[0].ptr = buffer;
    words[0].len = 0;

    uart.read([&](const rtl::u8& data){
      if (data == ' ' || data == '\r') {
        buffer[data_index++] = '\0';

        ++word_index;

        words[word_index].ptr = &buffer[data_index];
        words[word_index].len = 0;

        return (data == '\r');
      } else {
        buffer[data_index++] = data;
        words[word_index].len++;
        return false;
      }
    }).wait();

    /*
    if (memcmp(words[1].ptr, "drive", words[1].len) == 0) {
      gpio.drive_high();
    } else {
      gpio.drive_low();
    }
    */

    handler(words, word_index);
  }
  #endif
}
