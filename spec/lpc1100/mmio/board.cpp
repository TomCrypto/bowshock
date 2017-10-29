#define RTL_CORTEX_M0

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>

#include "simple_json.hpp"
#include "drivers/json.hpp"

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

constexpr auto TEST_ADDRESS = 0x10000FFC; // highest word in RAM
using TEST_REGISTER = rtl::mmio<TEST_ADDRESS, rtl::u32>;

auto run_spec(const test_params& params) {
  auto& variable = *reinterpret_cast<rtl::u32*>(TEST_ADDRESS);
  variable = params.initial_value;
  auto read_result = rtl::u32{};
  auto bit_result = bool{false};

  switch (params.operation) {
    case operation::masked_clear:
      TEST_REGISTER::clear<MASK>();
      break;
    case operation::clear:
      TEST_REGISTER::clear();
      break;
    case operation::masked_set:
      TEST_REGISTER::set<MASK>();
      break;
    case operation::set:
      TEST_REGISTER::set();
      break;
    case operation::toggle:
      TEST_REGISTER::toggle<MASK>();
      break;
    case operation::masked_write:
      TEST_REGISTER::write<MASK>(params.argument);
      break;
    case operation::safe_write:
      TEST_REGISTER::safe_write<MASK>(params.argument);
      break;
    case operation::write:
      TEST_REGISTER::write(params.argument);
      break;
    case operation::read:
      read_result = TEST_REGISTER::read<MASK>();
      break;
    case operation::any:
      bit_result = TEST_REGISTER::any<MASK>();
      break;
    case operation::all:
      bit_result = TEST_REGISTER::all<MASK>();
      break;
    case operation::none:
      bit_result = TEST_REGISTER::none<MASK>();
      break;
    case operation::clear_bit:
      TEST_REGISTER::clear_bit<BIT>();
      break;
    case operation::set_bit:
      TEST_REGISTER::set_bit<BIT>();
      break;
    case operation::toggle_bit:
      TEST_REGISTER::toggle_bit<BIT>();
      break;
    case operation::read_bit:
      bit_result = TEST_REGISTER::read_bit<BIT>();
      break;
  }

  return json::object{
    std::pair{"value", variable},
    std::pair{"read", read_result},
    std::pair{"bit", bit_result}
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
