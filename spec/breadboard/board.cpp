#define RTL_CORTEX_M0

#include <rtl/platform.hpp>

#include <hal/lpc1100/system.hpp>
#include <hal/lpc1100/digital_io.hpp>
#include <hal/lpc1100/uart.hpp>
#include <rtl/assert.hpp>
#include <hal/lpc1100/interrupt.hpp>
#include <hal/lpc1100/clock.hpp>

#include "protocols/event_list.hpp"

namespace dev = hal::lpc1100;

constexpr auto input_pin = dev::pin::PIO0_7;
constexpr auto output_pin = dev::pin::PIO0_8;

struct test_params {
  dev::digital_input<input_pin>::termination input_termination;
};

[[noreturn]] void main(const dev::reset_context& context) {
  auto spec = spec::event_list<dev::uart0, test_params>{9600_Hz};

  if (context.event == dev::reset_event::assert) {
    spec.fail(context.assert.message);
  }

  while (true) {
    spec.run([&](const auto& params){
      auto input = dev::digital_input<input_pin>{params.input_termination};

      {
        auto output = dev::digital_output<output_pin>{hal::logic_level::low};

        spec.event("driven low");
        output.drive_low();

        if (input.state() == hal::logic_level::low) {
          spec.event("read low");
        } else {
          spec.event("read high");
        }

        spec.event("driven high");
        output.drive_high();

        if (input.state() == hal::logic_level::low) {
          spec.event("read low");
        } else {
          spec.event("read high");
        }
      }

      {
        auto not_driven = dev::digital_input<output_pin>{dev::digital_input<output_pin>::termination::none};
        spec.event("not driven");

        if (input.state() == hal::logic_level::low) {
          spec.event("read low");
        } else {
          spec.event("read high");
        }
      }
    });
  }
}
