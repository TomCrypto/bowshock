require 'fiber'

module Protocols
  # Implements the event list protocol, which consists of writing a byte array
  # to the device containing test parameters and receiving in return a list of
  # newline-separated strings representing events logged by the device.
  class EventList
    def encode(bytes)
      bytes # TODO: encode length as first 4 bytes or something?
    end

    def decode
      make_fiber do
        bytes = []

        loop do
          byte = Fiber.yield
          break if byte.zero?
          bytes << byte
        end

        parse_events bytes
      end
    end

    private

    def parse_events(bytes)
      bytes.pack('C*').force_encoding('utf-8').split("\n")
    end

    def make_fiber(&block)
      fiber = Fiber.new(&block)
      fiber.resume
      fiber
    end
  end
end
