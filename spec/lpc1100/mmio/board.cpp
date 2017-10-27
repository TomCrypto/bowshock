#define RTL_CORTEX_M0

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>

#include "simple_json.hpp"
#include "json_driver.hpp"

namespace dev = hal::lpc1100;
namespace json = spec::json;

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

auto run_spec(const test_params& params) {
  auto variable = rtl::u32{params.initial_value};
  auto read_result = rtl::u32{};
  auto bit_result = bool{false};
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
      read_result = mmio.read<MASK>();
      break;
    case operation::any:
      bit_result = mmio.any<MASK>();
      break;
    case operation::all:
      bit_result = mmio.all<MASK>();
      break;
    case operation::none:
      bit_result = mmio.none<MASK>();
      break;
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
      bit_result = mmio.read_bit<BIT>();
      break;
  }

  return json::object{
    std::pair{"value", json::number{variable}},
    std::pair{"read", json::number{read_result}},
    std::pair{"bit", json::boolean{bit_result}}
  };
}

[[noreturn]] void main(const dev::reset_context& context) {
  auto spec = spec::json_driver<dev::uart0, test_params>{9600_Hz};

  if (context.event == dev::reset_event::assert) {
    spec.fail(context.assert.message);
  }

  while (true) {
    spec.run([&](auto&&... args) {
      return run_spec(std::forward<decltype(args)>(args)...);
    });
  }
}
