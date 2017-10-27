#define RTL_CORTEX_M0

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>

#include "simple_json.hpp"
#include "json_driver.hpp"

namespace dev = hal::lpc1100;
namespace json = spec::json;

constexpr auto input_pin = dev::pin::PIO0_7;
constexpr auto output_pin = dev::pin::PIO0_8;

struct test_params {
  dev::digital_input<input_pin>::termination input_termination;
};

auto logic_level_to_str(hal::logic_level level) {
  switch (level) {
    case hal::logic_level::low:
      return "low";
    case hal::logic_level::high:
      return "high";
  }
}

auto run_spec(const test_params& params) {
  auto state_when_driven_low = hal::logic_level{};
  auto state_when_driven_high = hal::logic_level{};
  auto state_when_not_driven = hal::logic_level{};

  auto input = dev::digital_input<input_pin>{params.input_termination};

  {
    auto output = dev::digital_output<output_pin>{hal::logic_level::low};
    state_when_driven_low = input.state();
  }

  {
    auto output = dev::digital_output<output_pin>{hal::logic_level::high};
    state_when_driven_high = input.state();
  }

  {
    auto not_driven = dev::digital_input<output_pin>{dev::digital_input<output_pin>::termination::none};
    state_when_not_driven = input.state();
  }

  return json::object{
    std::pair{"state_when_driven_low", json::string{logic_level_to_str(state_when_driven_low)}},
    std::pair{"state_when_driven_high", json::string{logic_level_to_str(state_when_driven_high)}},
    std::pair{"state_when_not_driven", json::string{logic_level_to_str(state_when_not_driven)}},
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
