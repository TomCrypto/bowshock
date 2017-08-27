#pragma once

namespace hal {

enum class clock_source {
  pll,
  main,
  core
};

template <clock_source source>
class clock;

template<>
class clock<pll> {
  enum class source {
    irc,
    oscillator
  };

  static void configure(const source& source, uint32_t frequency);

  //static set_source(const source& source);
};

//

}
