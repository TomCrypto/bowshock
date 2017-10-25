#pragma once

#include <rtl/base.hpp>
#include <sys/format.hpp>

namespace spec
{

template <typename T, typename Params> class event_list : private T
{
public:
  using T::T;

  template <typename Fn> auto run(Fn handler) {
    auto params = Params{};

    this->read(buffer(static_cast<rtl::u8*>(static_cast<void*>(&params)), sizeof(Params))).wait();
    handler(params);
    terminate_session();
  }

  // continuation function for a test that failed due to an assertion error or other hard fault
  auto fail(const char* message) {
    write_event(message);
    write_event("\n");
    terminate_session();
  }

  auto event(const char* message) {
    write_event(message);
    write_event("\n");
  }

private:
  auto write_event(const char* event) {
    this->write(sys::format(std::pair{"", event})).wait();
  }

  auto terminate_session() {
    this->write(byte(0x00)).wait();
  }

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
