#pragma once

/// @file
///
/// @brief UART implementation for the LPC1100 series microcontrollers.

#include <hal/interface.hpp>
#include <hal/lpc1100/physical_io.hpp>
#include <rtl/mmio.hpp>
#include <hal/lpc1100/interrupt.hpp>
#include <hal/lpc1100/clock.hpp>

// TODO: needs support for hardware flow control
// (let's not bother with auto-baud or modem features)

namespace hal::lpc1100 {

template <pin tx, pin rx> class uart : public hal::byte_interface<uart<tx, rx>> {
  friend void interrupt::handlers::uart(void);
private:
  using tx_pin_t = typename hal::lpc1100::physical_io<tx>;
  using rx_pin_t = typename hal::lpc1100::physical_io<rx>;
  tx_pin_t tx_pin{tx_pin_t::uart_tx_options::none};
  rx_pin_t rx_pin{rx_pin_t::uart_rx_options::none};

  using LSR = rtl::mmio_ro<0x40008014, rtl::u32>;
  using IER = rtl::mmio_rw<0x40008004, rtl::u32>;
  using FCR = rtl::mmio_wo<0x40008008, rtl::u32>;
  using RBR = rtl::mmio_ro<0x40008000, rtl::u8>;
  using THR = rtl::mmio_wo<0x40008000, rtl::u8>;
  using DLL = rtl::mmio_rw<0x40008000, rtl::u8>;
  using DLM = rtl::mmio_wo<0x40008004, rtl::u8>;
  using LCR = rtl::mmio_rw<0x4000800C, rtl::u8>;
  using FDR = rtl::mmio_rw<0x40008028, rtl::u8>;

public:
  template <typename T> uart(rtl::quantity<T, rtl::hertz> baud_rate) {
    configure_uart(baud_rate);

    interrupt::enable(interrupt::type::uart);
  }

  template <typename T> [[nodiscard]] auto write(const T& context) {
    return send_waitable<T>(context);
  }

  template <typename T> [[nodiscard]] auto read(const T& context) {
    return recv_waitable<T>(context);
  }

  ~uart() {
    interrupt::disable(interrupt::type::uart);
    clock<clock_source::uart>::disable();

    rtl::assert(!send_context.valid(), TRACE("still sending"));
    rtl::assert(!recv_context.valid(), TRACE("still receiving"));
  }

private:
  static inline rtl::interrupt_context<> send_context{};
  static inline rtl::interrupt_context<rtl::u32> recv_context{};

  template <typename T> auto configure_uart(T baud_rate) {
    clock<clock_source::uart>::enable(); // divider is 1

    auto clock_hertz = clock<clock_source::uart>::frequency<rtl::u32>().in<rtl::hertz>();
    auto divisor = (clock_hertz / 16 / baud_rate).template as<rtl::dimensionless>();

    LCR::write(0b10000000); // enable latches

    DLM::write(divisor / 256);
    DLL::write(divisor % 256);

    FDR::write(0b00010000); // mul = 1, divadd = 0 (no fractional divider)
    LCR::write(0b00000011); // 8 bit words, 1 stop bit, no parity, disable latches

    FCR::set<0b11000111>(); // 2 MSBs are the RX FIFO trigger level (here set to 0b11 = 14 characters)
  }

  template <typename T> struct send_waitable : private rtl::noncopyable {
  private:
    auto flush_tx_queue() {
      FCR::set<0b100>();
    }
  
    auto fill_tx_queue() {
      for (auto i = 0; i < 16; ++i) {
        rtl::u8 data;

        auto result = context(data);

        if (result == rtl::waitable::status::pending) {
          THR::write(data);
        } else {
          return std::pair{i != 0, result};
        }
      }
  
      return std::pair{true, rtl::waitable::status::pending};
    }

  public:
    send_waitable(const T& context) : status(rtl::waitable::status::pending), context(context) {
      send_context = {decltype(send_context)::template member_function<send_waitable<T>>, this};

      flush_tx_queue();

      auto result = fill_tx_queue();
      status = result.second;

      switch (status) {
        case rtl::waitable::status::pending:
          LSR::read();
          IER::set<0b10>();
          break;
        case rtl::waitable::status::complete:
          if (result.first) {
            completing = true;
            status = rtl::waitable::status::pending;
            LSR::read();
            IER::set<0b10>();
          }
          break;
        case rtl::waitable::status::failed:
          IER::clear<0b10>();
      }
    }

    send_waitable(send_waitable<T>&& other) = delete;
    send_waitable<T>& operator=(send_waitable&& other) = delete;

    // TODO: if we define move semantics, need possible interrupt disable/enable for safely moving the lambda

    ~send_waitable() {
      IER::clear<0b10>();
      send_context.reset();
    }

    auto is_complete() const {
      return status == rtl::waitable::status::complete;
    }

    auto is_failed() const {
      return status == rtl::waitable::status::failed;
    }

    auto is_pending() const {
      return status == rtl::waitable::status::pending;
    }

    auto interrupt() {
      if (completing) {
        status = rtl::waitable::status::complete;
        IER::clear<0b10>();
        return;
      }

      auto result = fill_tx_queue();

      if (result.second != rtl::waitable::status::pending) {
        if (result.second == rtl::waitable::status::complete) {
          if (result.first) {
            completing = true;
          } else {
            IER::clear<0b10>();
            status = result.second;
          }
        } else {
          IER::clear<0b10>();
          status = result.second;
        }
      }
    }

    auto wait() const {
      return rtl::waitable::wait_all(*this);
    }

  private:
    rtl::waitable::status status;
    bool completing{false};
    T context;
  };

  template <typename T> struct recv_waitable : private rtl::noncopyable {
  private:
    auto flush_rx_queue() {
      FCR::set<0b010>();
    }

  public:
    recv_waitable(const T& context) : status(rtl::waitable::status::pending), context(context) {
      recv_context = {decltype(recv_context)::template member_function<recv_waitable<T>>, this};

      flush_rx_queue();
      LSR::read();
      IER::set<0b101>();
    }

    recv_waitable(recv_waitable<T>&& other) = delete;
    recv_waitable<T>& operator=(recv_waitable&& other) = delete;

    // TODO: if we define move semantics, need possible interrupt disable/enable for safely moving the lambda

    ~recv_waitable() {
      recv_context.reset();
    }

    auto is_complete() const {
      return status == rtl::waitable::status::complete;
    }

    auto is_failed() const {
      return status == rtl::waitable::status::failed;
    }

    auto is_pending() const {
      return status == rtl::waitable::status::pending;
    }

    auto interrupt(rtl::u32 iir) {
      switch (iir) {
        case 0b0110: /* RLS */
          if (LSR::any<0b10001110>()) {
            status = rtl::waitable::status::failed;
          }

          break;
        case 0b0100: /* RDA */
        case 0b1100: /* CTI */
          while (LSR::all<0b1>()) {
            auto result = context(RBR::read());

            if (result != rtl::waitable::status::pending) {
              status = result;
              break;
            }
          }
      }
    }

    auto wait() const {
      return rtl::waitable::wait_all(*this);
    }

  private:
    rtl::waitable::status status;
    T context;
  };
};

using uart0 = uart<pin::TXD, pin::RXD>;

inline void interrupt::handlers::uart(void) {
  auto iir = rtl::mmio_ro<0x40008008, rtl::u32>::read<0b1110>();

  switch (iir) {
    case 0b0010:
      return uart0::send_context();
    default:
      return uart0::recv_context(iir);
  }
}

}
