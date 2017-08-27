#include <rtl/platform.hpp>

#include <hal/lpc1114/headers/LPC11xx.h>
#include <hal/lpc1114/headers/hdr_syscon.h>

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/interface.hpp>
#include <rtl/assert.hpp>

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

void main(const dev::reset_context& context) {
  if (context.event == dev::reset_event::assert) {
    assert_signal();
  }

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

  auto output = output_pin(hal::logic_level::high);
  auto input = input_pin(input_pin::termination::pullup);

  //auto gpio2 = dev::gpio2<dev::pin::PIO0_8>(/* termination, interrupt settings, etc.. */);
  //auto uart = dev::uart<dev::pin::PIO1_7, dev::pin::PIO1_6>(/* uart settings */);

  auto foo = [&](){
  while (true) {
    /*
    for (auto i = 0; i < 0x1FFFFF; ++i) {
        output.drive_low();
    }

    for (auto i = 0; i < 0x1FFFFF; ++i) {
        output.drive_high();
    }
    */

    output.drive(input.state());

    rtl::assert(input.state() == hal::logic_level::high, "fuck");
}};

foo();

  return;
}

static interrupt void HardFault_Handler(void)
{
    return;
}

extern "C" interrupt void __attribute__((used)) __Default_Handler(void)
{
    return;
}

// Reserved 0x08
void __Reserved_0x08_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x10
void __Reserved_0x10_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x14
void __Reserved_0x14_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x18
void __Reserved_0x18_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x1C
void __Reserved_0x1C_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x20
void __Reserved_0x20_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x24
void __Reserved_0x24_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x28
void __Reserved_0x28_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x30
void __Reserved_0x30_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x34
void __Reserved_0x34_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_0 interrupt
void WAKEUP_PIO0_0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_1 interrupt
void WAKEUP_PIO0_1_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_2 interrupt
void WAKEUP_PIO0_2_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_3 interrupt
void WAKEUP_PIO0_3_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_4 interrupt
void WAKEUP_PIO0_4_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_5 interrupt
void WAKEUP_PIO0_5_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_6 interrupt
void WAKEUP_PIO0_6_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_7 interrupt
void WAKEUP_PIO0_7_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_8 interrupt
void WAKEUP_PIO0_8_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_9 interrupt
void WAKEUP_PIO0_9_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_10 interrupt
void WAKEUP_PIO0_10_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO0_11 interrupt
void WAKEUP_PIO0_11_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Start logic wake-up from PIO1_0 interrupt
void WAKEUP_PIO1_0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// C_CAN interrupt (LPC11C12/C14 only)
void CAN_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// SPI/SSP1 interrupt
void SSP1_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// I2C interrupt
void I2C_IRQHandler(void) {
  return;
}

// CT16B0 (16-bit Timer0) interrupt
void TIMER16_0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// CT16B1 (16-bit Timer1) interrupt
void TIMER16_1_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// CT32B0 (32-bit Timer0) interrupt
void TIMER32_0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));
//extern void TIMER32_0_IRQHandler(void);

// CT32B1 (32-bit Timer1) interrupt
void TIMER32_1_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// SPI/SSP0 interrupt
void SSP0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

void UART_IRQHandler(void) {
  return;
}

// Reserved 0x98
void __Reserved_0x98_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0x9C
void __Reserved_0x9C_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// A/D Converter interrupt
void ADC_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Watchdog timer interrupt
void WDT_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Brown Out Detect interrupt
void BOD_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// Reserved 0xAC
void __Reserved_0xAC_Handler(void) __attribute__ ((weak, alias("__Default_Handler")));

// PIO INT3 interrupt
void PIOINT3_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// PIO INT2 interrupt
void PIOINT2_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// PIO INT1 interrupt
void PIOINT1_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

// PIO INT0 interrupt
void PIOINT0_IRQHandler(void) __attribute__ ((weak, alias("__Default_Handler")));

extern const char __LD_STACK_TOP;

extern "C" void rtl_init(void);

void (* const vectors[])(void) __attribute__((used)) section(".vect") = {
        (void (*)(void))&__LD_STACK_TOP, // Main stack end address
        rtl_init,                           // Reset
        __Reserved_0x08_Handler,            // Reserved 0x08
        HardFault_Handler,                  // All class of fault
        __Reserved_0x10_Handler,            // Reserved 0x10
        __Reserved_0x14_Handler,            // Reserved 0x14
        __Reserved_0x18_Handler,            // Reserved 0x18
        __Reserved_0x1C_Handler,            // Reserved 0x1C
        __Reserved_0x20_Handler,            // Reserved 0x20
        __Reserved_0x24_Handler,            // Reserved 0x24
        __Reserved_0x28_Handler,            // Reserved 0x28
        __Default_Handler,                        // System service call via SWI instruction
        __Reserved_0x30_Handler,            // Reserved 0x30
        __Reserved_0x34_Handler,            // Reserved 0x34
        #if 0
        ReleaseTask,                         // Pendable request for system service
        #endif
        __Default_Handler,
        __Default_Handler,                         // System tick timer
        WAKEUP_PIO0_0_IRQHandler,           // Start logic wake-up from PIO0_0 interrupt
        WAKEUP_PIO0_1_IRQHandler,           // Start logic wake-up from PIO0_1 interrupt
        WAKEUP_PIO0_2_IRQHandler,           // Start logic wake-up from PIO0_2 interrupt
        WAKEUP_PIO0_3_IRQHandler,           // Start logic wake-up from PIO0_3 interrupt
        WAKEUP_PIO0_4_IRQHandler,           // Start logic wake-up from PIO0_4 interrupt
        WAKEUP_PIO0_5_IRQHandler,           // Start logic wake-up from PIO0_5 interrupt
        WAKEUP_PIO0_6_IRQHandler,           // Start logic wake-up from PIO0_6 interrupt
        WAKEUP_PIO0_7_IRQHandler,           // Start logic wake-up from PIO0_7 interrupt
        WAKEUP_PIO0_8_IRQHandler,           // Start logic wake-up from PIO0_8 interrupt
        WAKEUP_PIO0_9_IRQHandler,           // Start logic wake-up from PIO0_9 interrupt
        WAKEUP_PIO0_10_IRQHandler,          // Start logic wake-up from PIO0_10 interrupt
        WAKEUP_PIO0_11_IRQHandler,          // Start logic wake-up from PIO0_11 interrupt
        WAKEUP_PIO1_0_IRQHandler,           // Start logic wake-up from PIO1_0 interrupt
        CAN_IRQHandler,                     // C_CAN interrupt (LPC11C12/C14 only)
        SSP1_IRQHandler,                    // SPI/SSP1 interrupt
        I2C_IRQHandler,                     // I2C interrupt
        TIMER16_0_IRQHandler,               // CT16B0 (16-bit Timer0) interrupt
        TIMER16_1_IRQHandler,               // CT16B1 (16-bit Timer1) interrupt
        TIMER32_0_IRQHandler,               // CT32B0 (32-bit Timer0) interrupt
        TIMER32_1_IRQHandler,               // CT32B1 (32-bit Timer1) interrupt
        SSP0_IRQHandler,                    // SPI/SSP0 interrupt
        UART_IRQHandler,                    // UART interrupt
        __Reserved_0x98_Handler,            // Reserved 0x98
        __Reserved_0x9C_Handler,            // Reserved 0x9C
        ADC_IRQHandler,                     // A/D Converter interrupt
        WDT_IRQHandler,                     // Watchdog timer interrupt
        BOD_IRQHandler,                     // Brown Out Detect interrupt
        __Reserved_0xAC_Handler,            // Reserved 0xAC
        PIOINT3_IRQHandler,                 // PIO INT3 interrupt
        PIOINT2_IRQHandler,                 // PIO INT2 interrupt
        PIOINT1_IRQHandler,                 // PIO INT1 interrupt
        PIOINT0_IRQHandler,                 // PIO INT0 interrupt
};
