require 'rubyserial'
require 'timeout'

module Links
  # This implements a basic byte-oriented link over a serial port.
  class Serial
    NAME = 'serial'.freeze

    def initialize(port:, baud_rate:, timeout: 3)
      @port = port
      @baud_rate = baud_rate
      @timeout = timeout
    end

    def interact(bytes)
      Timeout.timeout(@timeout) do
        write bytes
        read final: 0x00
      end
    rescue Timeout::Error
      raise "no response on #{@port}"
    end

    def stop
      serial.close
    end

    private

    def serial
      @serial ||= ::Serial.new @port, @baud_rate, 8, :none
    end

    def write(bytes)
      serial.write(bytes.pack('C*'))
    end

    # read N bytes or until a byte is reached
    # TODO: this logic should be moved out
    def read(count: 0, final: nil)
      data = []

      loop do
        byte = serial.getbyte
        next if byte.nil?
        break if byte == final || data.length == count - 1

        data << byte
      end

      data
    end
  end
end
