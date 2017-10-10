#pragma once

/// @file
///
/// @brief Implementation of the Standard spec protocol.

#include <rtl/base.hpp>
#include <sys/format.hpp>

namespace sys::spec
{

template <typename T, typename Params> class standard : private T
{
public:
  using T::T;

  template <typename Fn> auto run(Fn handler) {
    auto params = Params{};

    this->read(buffer(static_cast<rtl::u8*>(static_cast<void*>(&params)), sizeof(Params))).wait();
    handler(params);
    this->write(sys::format(std::pair{"", "ok"})).wait();
    this->write(byte(0x00)).wait();
  }

  // continuation function for a test that failed due to an assertion error or other hard fault
  auto fail(const char* message) {
    this->write(sys::format(std::pair{"", message})).wait();
    this->write(sys::format(std::pair{"", "\nfail"})).wait();
    this->write(byte(0x00)).wait();
  }

  auto event(const char* message) {
    this->write(sys::format(std::pair{"", message})).wait();
    this->write(sys::format(std::pair{"", "\n"})).wait();
  }

private:
  auto byte(rtl::u8 byte) {
    return [byte, written = false](rtl::u8& data) mutable {
      if (written) {
        return rtl::waitable::status::complete;
      } else {
        data = byte;
        written = true;
        return rtl::waitable::status::pending;
      }
    };
  }

  auto until(rtl::u8 byte) {
    return [byte](const rtl::u8& data) {
      if (data == byte) {
        return rtl::waitable::status::complete;
      } else {
        return rtl::waitable::status::pending;
      }
    };
  }

  auto buffer(rtl::u8* buffer, std::size_t length) {
    return [buffer, length](const rtl::u8& data) mutable {
      length--;
      *(buffer++) = data;

      if (length == 0) {
        return rtl::waitable::status::complete;
      } else {
        return rtl::waitable::status::pending;
      }
    };
  }
};

}
