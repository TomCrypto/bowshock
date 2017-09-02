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

  static auto LSR() { return rtl::mmio_ro<rtl::u32>{0x40008014}; }
  static auto IER() { return rtl::mmio_rw<rtl::u32>{0x40008004}; }
  static auto FCR() { return rtl::mmio_wo<rtl::u32>{0x40008008}; }
  static auto RBR() { return rtl::mmio_ro<rtl::u8>{0x40008000}; }
  static auto THR() { return rtl::mmio_wo<rtl::u8>{0x40008000}; }
  static auto DLL() { return rtl::mmio_wo<rtl::u8>{0x40008000}; }
  static auto DLM() { return rtl::mmio_wo<rtl::u8>{0x40008004}; }

public:
  uart(int baud_rate) {
    configure_uart(baud_rate, 48000000); // TODO: hardcode clock for now

    interrupt::enable(interrupt::type::uart);
  }

  template <typename T> auto write(const T& context) {
    return send_waitable<T>(context);
  }

  template <typename T> auto read(const T& context) {
    return recv_waitable<T>(context);
  }

  ~uart() {
    interrupt::disable(interrupt::type::uart);

    // TODO: cancel both waitables, if present
    //send.status = rtl::waitable<>::status::failed;
    //recv.status = rtl::waitable<>::status::failed;
  }

private:
  static inline rtl::interrupt_context<> send_context{};
  static inline rtl::interrupt_context<rtl::u32> recv_context{};

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

  template <typename T> struct send_waitable : public rtl::waitable<send_waitable<T>>, private rtl::noncopyable {
  private:
    auto flush_tx_queue() {
      FCR().template set<0b100>();
    }
  
    auto fill_tx_queue() {
      for (auto i = 0; i < 16; ++i) {
        rtl::u8 data;
  
        if (context(data)) {
          return true;
        }
  
        THR().write(data);
      }
  
      return false;
    }
  
    enum class status {
      pending,
      failed,
      complete,
      finishing
    };
  
  public:
    send_waitable(const T& context) : status(status::pending), context(context) {
      send_context = {decltype(send_context)::template member_function<send_waitable<T>>, this};

      flush_tx_queue();

      if (fill_tx_queue()) {
        status = status::finishing;
      }
  
      LSR().read();
      IER().template set<0b10>();
    }

    send_waitable(send_waitable<T>&& other) = delete;
    send_waitable<T>& operator=(send_waitable&& other) = delete;

    // TODO: if we define move semantics, need possible interrupt disable/enable for safely moving the lambda

    ~send_waitable() {
      IER().template clear<0b10>();
      send_context.reset();
    }

    auto is_complete() const {
      return status == status::complete;
    }

    auto is_failed() const {
      return status == status::failed;
    }

    auto is_pending() const {
      return status == status::pending || status == status::finishing;
    }

    auto interrupt() {
      if (status == status::finishing) {
        status = status::complete;
        IER().template clear<0b10>();
      } else {
        if (fill_tx_queue()) {
          status = status::finishing;
        }
      }
    }

  private:
    status status;
    T context;
  };

  template <typename T> struct recv_waitable : public rtl::waitable<recv_waitable<T>>, private rtl::noncopyable {
  private:
    auto flush_rx_queue() {
      FCR().template set<0b010>();
    }

    enum class status {
      pending,
      failed,
      complete
    };

  public:
    recv_waitable(const T& context) : status(status::pending), context(context) {
      recv_context = {decltype(recv_context)::template member_function<recv_waitable<T>>, this};

      flush_rx_queue();
      LSR().read();
      IER().template set<0b101>();
    }

    recv_waitable(recv_waitable<T>&& other) = delete;
    recv_waitable<T>& operator=(recv_waitable&& other) = delete;

    // TODO: if we define move semantics, need possible interrupt disable/enable for safely moving the lambda

    ~recv_waitable() {
      recv_context.reset();
    }

    auto is_complete() const {
      return status == status::complete;
    }

    auto is_failed() const {
      return status == status::failed;
    }

    auto is_pending() const {
      return status == status::pending;
    }

    auto interrupt(rtl::u32 iir) {
      switch (iir) {
        case 0b0110: /* RLS */
          if (LSR().template any<0b10001110>()) {
            status = status::failed;
          }

          break;
        case 0b0100: /* RDA */
        case 0b1100: /* CTI */
          while (LSR().template all<0b1>()) {
            if (context(RBR().read())) {
              status = status::complete;
              IER().template clear<0b101>();
              break;
            }
          }
      }
    }

  private:
    status status;
    T context;
  };
};

using uart0 = uart<pin::TXD, pin::RXD>;

inline void interrupt::handlers::uart(void) {
  auto iir = rtl::mmio_ro<rtl::u32>{0x40008008}.template read<0b1110>();

  switch (iir) {
    case 0b0010:
      return uart0::send_context();
    default:
      return uart0::recv_context(iir);
  }
}

}
