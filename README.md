Proof of concept in implementing a modern C++ HAL for embedded projects.

Currently a work in progress with the LPC1100 series microcontrollers (such as the LPC1114) being used as the first target.

Documented with Doxygen (WIP).

Known to work with `arm-none-eabi-gcc` 7.2.0 and LLVM/Clang 5.0.0.

Testing
-------

Hardware tests are run directly on the device and are driven with RSpec. Each test consists of running an *example* (an expectation or set of expectations) on a *board*, which is an abstract representation of some hardware circuit. The circuit board is connected to the host via *links*, which can be various types of interfaces, ranging from serial ports to GPIO drivers and I2C interfaces. Each link is used either for uploading the program being tested onto the device, or for communicating test parameters and test results to/from the device, or for implementing hardware stubs for testing purposes, such as running an echo server over a serial port, or checking the state of a GPIO pin on the board.

The specs for each board are located in the `spec/$BOARDNAME` folder, which contains at least four files:

 1. `board_spec.rb` is the RSpec file that will be running the tests on the board
 2. `board.rb` is the Ruby abstraction RSpec uses to communicate with the board
 3. `board.cpp` is the C++ implementation of the program running on the device
 4. `board_links.yml` is a configuration file describing the different links

The `board_links.yml` file is not in source control, since the link configuration, such as which serial port to use, depends on the way the board is hooked up to the host, which is environment-specific. It should be provided if you want to run the spec for a board, and if it is absent, the spec is skipped. For instance, the following configuration defines a serial link called `main` over port COM3 at 9600 baud:

```yaml
main:
  type: serial
  args:
    port: COM3
    baud_rate: 9600
```

The different link types and their arguments can be found in `spec/support/links`, and the link types expected by a given spec are documented in the spec folder.
