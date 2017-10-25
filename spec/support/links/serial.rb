require 'rubyserial'
require 'timeout'

module Links
  # Implements a byte-oriented link over a serial port.
  class Serial
    NAME = 'serial'.freeze

    def initialize(port:, baud_rate:, timeout: 3)
      @port = port
      @baud_rate = baud_rate
      @timeout = timeout
    end

    def write(bytes)
      serial.write(bytes.pack('C*'))
    end

    def read(count)
      Timeout.timeout(@timeout) do
        data = []

        loop do
          break data if data.length == count
          byte = serial.getbyte # one byte
          data << byte unless byte.nil?
        end
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
  end
end
