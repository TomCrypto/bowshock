#pragma once

/// @file
///
/// @brief UART implementation for the LPC1100 series microcontrollers.

#include <hal/interface.hpp>
#include <hal/lpc1100/physical_io.hpp>
#include <rtl/mmio.hpp>
#include <hal/lpc1100/interrupt.hpp>

// TODO: for the clocks, potentially the UART constructor could take a "clock" object that it can attach the UART clock
//       to, and also query to obtain its actual clock rate, so that the correct UART parameters can be constructed.

namespace hal::lpc1100 {

template <pin tx, pin rx> class uart : public hal::byte_interface<uart<tx, rx>> {
  friend void interrupt::handlers::uart(void);
private:
  using tx_pin_t = typename hal::lpc1100::physical_io<tx>;
  using rx_pin_t = typename hal::lpc1100::physical_io<rx>;
  tx_pin_t tx_pin{tx_pin_t::uart_tx_options::none};
  rx_pin_t rx_pin{rx_pin_t::uart_rx_options::none};

  struct {
    rtl::waitable<>::status status;
    rtl::linear_reader<rtl::u8> reader;
  } send{};

  struct {
    rtl::waitable<>::status status;
    rtl::linear_writer<rtl::u8> writer;
  } recv{};

  static auto LSR() { return rtl::mmio_ro<rtl::u32>{0x40008014}; }
  static auto IER() { return rtl::mmio_rw<rtl::u32>{0x40008004}; }
  static auto FCR() { return rtl::mmio_wo<rtl::u32>{0x40008008}; }
  static auto IIR() { return rtl::mmio_ro<rtl::u32>{0x40008008}; }
  static auto RBR() { return rtl::mmio_ro<rtl::u8>{0x40008000}; }
  static auto THR() { return rtl::mmio_wo<rtl::u8>{0x40008000}; }
  static auto DLL() { return rtl::mmio_wo<rtl::u8>{0x40008000}; }
  static auto DLM() { return rtl::mmio_wo<rtl::u8>{0x40008004}; }

public:
  static uart<tx, rx>* current;

  uart(int baud_rate) {
    rtl::assert(current == nullptr, TRACE("UART is already in use!"));
    current = this;

    configure_uart(baud_rate, 48000000); // TODO: hardcode clock for now

    interrupt::enable(interrupt::type::uart);
  }

  auto write_start() {
    flush_tx_queue();
    fill_tx_queue();
    LSR().read();
    IER().template set<0b10>();
  }

  auto write_end() {
    IER().template clear<0b10>();
  }

  auto cancel_write() {
    rtl::assert(IER().template all<0b10>(), TRACE("UART is not sending"));
    send.status = rtl::waitable<>::status::failed;
    write_end();
  }

  auto write(rtl::buffer buffer) {
    rtl::assert(IER().template none<0b10>(), TRACE("UART is already sending"));

    if (buffer.size() == 0) {
      send.status = rtl::waitable<>::status::complete;
      return rtl::interrupt_waitable(&send.status);
    }

    send.reader = rtl::linear_reader<rtl::u8>{buffer};
    send.status = rtl::waitable<>::status::pending;
    write_start();

    return rtl::interrupt_waitable(&send.status);
  }

  auto read_start() {
    flush_rx_queue();
    LSR().read();
    IER().template set<0b101>();
  }

  auto read_end() {
    IER().template clear<0b101>();
  }

  auto cancel_read() {
    rtl::assert(IER().template all<0b101>(), TRACE("UART is not receiving"));
    recv.status = rtl::waitable<>::status::failed;
    read_end();
  }

  auto read(rtl::buffer buffer) {
    rtl::assert(IER().template none<0b101>(), TRACE("UART is already receiving"));

    if (buffer.size() == 0) {
      recv.status = rtl::waitable<>::status::complete;
      return rtl::interrupt_waitable(&recv.status);
    }

    recv.writer = rtl::linear_writer<rtl::u8>{buffer};
    recv.status = rtl::waitable<>::status::pending;
    read_start();

    return rtl::interrupt_waitable(&recv.status);
  }

  ~uart() {
    interrupt::disable(interrupt::type::uart);

    send.status = rtl::waitable<>::status::failed;
    recv.status = rtl::waitable<>::status::failed;

    current = nullptr;
  }

private:
  auto interrupt() {
    switch (IIR().template read<0b1110>()) {
      case 0b0010: /* THRE */
        if (fill_tx_queue()) {
          send.status = rtl::waitable<>::status::complete;
          write_end();
        }

        break;
      case 0b0110: /* RLS */
        if (!LSR().template all<0b10000>()) {
          recv.status = rtl::waitable<>::status::failed;
          read_end();
        }

        break;
      case 0b0100: /* RDA */
      case 0b1100: /* CTI */
        while (LSR().template all<0b1>()) {
          auto byte = RBR().read();

          recv.writer.write(byte);

          if (recv.writer.end_reached()) {
            recv.status = rtl::waitable<>::status::complete;
            read_end();
          }
        }
    }
  }

  auto flush_rx_queue() {
    FCR().template set<0b10>();
  }

  auto flush_tx_queue() {
    FCR().template set<0b100>();
  }

  auto fill_tx_queue() {
    for (auto i = 0; i < 16; ++i) {
      if (send.reader.end_reached()) {
        return i == 0;
      }

      THR().write(send.reader.read());
    }

    return false;
  }

  auto configure_uart(int baud_rate, int core_clock) {
    unsigned int regVal;
    unsigned int Fdiv;

    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);    //Enable UART clock
    LPC_SYSCON->UARTCLKDIV = 0x01;    //Divide by 1

    LPC_UART->LCR = 0x83;           //8 bits, no parity, 1 stop bit, DLAB(divisor latch access bit) = 1
    regVal = LPC_SYSCON->UARTCLKDIV;
    Fdiv = (((core_clock)/regVal)/16)/baud_rate;    /*baud rate */

    DLM().write(Fdiv / 256);
    DLL().write(Fdiv % 256);

    LPC_UART->FDR = 0x00 | (1 << 4) | 0;
    LPC_UART->LCR = 0x03;        /* Diable latch access bit DLAB = 0 */
    LPC_UART->FCR = 0x07 | (0x3 << 6);        /* Enable and reset TX and RX FIFO. */

    //LPC_UART->FCR = (1 << 2);
  }
};

template <pin tx, pin rx>
uart<tx, rx>* uart<tx, rx>::current;

using uart0 = uart<pin::TXD, pin::RXD>;

inline void interrupt::handlers::uart(void) {
  hal::lpc1100::uart<hal::lpc1100::pin::TXD, hal::lpc1100::pin::RXD>::current->interrupt();
}

}
