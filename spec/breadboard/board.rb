# Links expected:
#   device => program upload link to device
#   main => serial link to device UART0

class Breadboard
  def initialize(options, links)
    @options = options
    @links = links
  end

  def upload(program)
    device.upload program
  end

  def status
    results.last.to_sym
  end

  def events
    results[0..-2]
  end

  private

  def device
    @links[:device]
  end

  def main
    @links[:main]
  end

  def unpack_bytes(bytes)
    bytes.pack('C*').force_encoding('utf-8').split("\n")
  end

  def results
    unpack_bytes main.interact(Parameters.new(params).bytes)
  end

  def params
    @params ||= {
      termination: TERMINATIONS.fetch(@options.fetch(:termination))
    }
  end

  class Parameters < BinaryStruct
    layout :termination, :uint
  end

  TERMINATIONS = {
    floating: 0 << 3,
    pulldown: 1 << 3,
    pullup: 2 << 3,
    repeater: 3 << 3
  }.freeze
end
