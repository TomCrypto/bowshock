require 'open3'

module Links
  # This flasher calls into the lpc21isp command-line tool to flash an LPC ARM
  # microcontroller from a serial port. The tool must be in the user's PATH.
  class LPC21ISP
    NAME = 'lpc21isp'.freeze

    def initialize(port:, oscillator_frequency:,
                   baud_rate: 115_200, retries: 3)
      @port = port
      @baud_rate = baud_rate
      @oscillator_frequency = oscillator_frequency
      @retries = retries
    end

    def upload(program)
      DeviceCache.upload_program @port, program do
        output, status = Open3.capture2e command_arguments(program)
        raise "upload failed: \n\n#{output}" unless status.success?
      end
    end

    def stop; end

    private

    def command_arguments(program)
      [
        'lpc21isp', '-control', "-try#{@retries}", '-bin',
        program, @port, @baud_rate, @oscillator_frequency
      ].join ' '
    end
  end
end
