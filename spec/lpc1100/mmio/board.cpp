#define RTL_CORTEX_M0

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>

#include "protocols/event_list.hpp"

namespace dev = hal::lpc1100;

enum class operation : rtl::u32 {
  masked_clear    = 0,
  clear           = 1,
  masked_set      = 2,
  set             = 3,
  toggle          = 4,
  masked_write    = 5,
  safe_write      = 6,
  write           = 7,
  read            = 8,
  any             = 9,
  all             = 10,
  none            = 11,
  clear_bit       = 12,
  set_bit         = 13,
  toggle_bit      = 14,
  read_bit        = 15
};

// These constants are also referenced in the corresponding spec file
constexpr auto MASK = rtl::u32{0b11010101111110010011101100011011};
constexpr auto BIT  = rtl::u32{19};

struct test_params {
  operation operation;
  rtl::u32 initial_value;
  rtl::u32 argument;
};

auto to_binary_string(rtl::u32 value) {
  static char buffer[35] = "0b";

  for (auto i = 0; i < 32; ++i) {
    buffer[33 - i] = (value & 0b1) ? '1' : '0';
    value >>= 1;
  }

  return static_cast<const char*>(buffer);
}

auto run_spec(spec::event_list<dev::uart0, test_params>& spec, const test_params& params) {
  auto variable = rtl::u32{params.initial_value};
  auto mmio = rtl::mmio<rtl::u32>{reinterpret_cast<rtl::uptr>(&variable)};

  switch (params.operation) {
    case operation::masked_clear:
      mmio.clear<MASK>();
      break;
    case operation::clear:
      mmio.clear();
      break;
    case operation::masked_set:
      mmio.set<MASK>();
      break;
    case operation::set:
      mmio.set();
      break;
    case operation::toggle:
      mmio.toggle<MASK>();
      break;
    case operation::masked_write:
      mmio.write<MASK>(params.argument);
      break;
    case operation::safe_write:
      mmio.safe_write<MASK>(params.argument);
      break;
    case operation::write:
      mmio.write(params.argument);
      break;
    case operation::read:
      variable = mmio.read<MASK>();
      break;
    case operation::any:
      spec.event(mmio.any<MASK>() ? "true" : "false");
      return;
    case operation::all:
      spec.event(mmio.all<MASK>() ? "true" : "false");
      return;
    case operation::none:
      spec.event(mmio.none<MASK>() ? "true" : "false");
      return;
    case operation::clear_bit:
      mmio.clear_bit<BIT>();
      break;
    case operation::set_bit:
      mmio.set_bit<BIT>();
      break;
    case operation::toggle_bit:
      mmio.toggle_bit<BIT>();
      break;
    case operation::read_bit:
      spec.event(mmio.read_bit<BIT>() ? "true" : "false");
      return;
  }

  spec.event(to_binary_string(variable));
}

[[noreturn]] void main(const dev::reset_context& context) {
  auto spec = spec::event_list<dev::uart0, test_params>{9600_Hz};

  if (context.event == dev::reset_event::assert) {
    spec.fail(context.assert.message);
  }

  while (true) {
    spec.run([&](auto&&... args) {
      run_spec(spec, std::forward<decltype(args)>(args)...);
    });
  }
}
