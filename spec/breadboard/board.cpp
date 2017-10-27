#define RTL_CORTEX_M0

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>

#include "simple_json.hpp"
#include "drivers/json.hpp"

namespace dev = hal::lpc1100;
namespace json = spec::json;

constexpr auto input_pin = dev::pin::PIO0_7;
constexpr auto output_pin = dev::pin::PIO0_8;

struct test_params {
  dev::digital_input<input_pin>::termination input_termination;
};

namespace spec::json
{

template <> struct json_type_for<hal::logic_level> { using type = string; };
template <> string to_json_type(hal::logic_level element) {
  return element == hal::logic_level::high ? "high" : "low";
}

}

auto drive_low_and_check(dev::digital_input<input_pin>& input) {
  auto output = dev::digital_output<output_pin>{hal::logic_level::low};
  return input.state();
}

auto drive_high_and_check(dev::digital_input<input_pin>& input) {
  auto output = dev::digital_output<output_pin>{hal::logic_level::high};
  return input.state();
}

auto dont_drive_and_check(dev::digital_input<input_pin>& input) {
  // we need to introduce a little delay between no longer driving the output pin
  // and checking the state of the input pin, as the microcontroller pulldowns and
  // pullups are quite weak and will take a while to raise or lower the voltage
  // (especially if this is done on a breadboard with high wire capacitance)
  for (volatile auto dummy = 0; dummy < 100; ++dummy);
  return input.state();
}

auto run_spec(const test_params& params) {
  auto input = dev::digital_input<input_pin>{params.input_termination};

  auto state_when_driven_low = drive_low_and_check(input);
  auto state_when_not_driven = dont_drive_and_check(input);
  auto state_when_driven_high = drive_high_and_check(input);
  auto state_when_not_driven_again = dont_drive_and_check(input);

  return json::object{
    std::pair{"state_when_driven_low", state_when_driven_low},
    std::pair{"state_when_not_driven", state_when_not_driven},
    std::pair{"state_when_driven_high", state_when_driven_high},
    std::pair{"state_when_not_driven_again", state_when_not_driven_again},
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
