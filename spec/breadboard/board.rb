# Links expected:
#   device => program upload link to device
#   main => serial link to device UART0

class Breadboard < JSONDriver
  def initialize(options, links)
    @options = options
    @links = links
  end

  def upload(program)
    device.upload program
  end

  private

  def device
    @links[:device]
  end

  def link
    @links[:main]
  end

  def params
    @params ||= {
      termination: TERMINATIONS.fetch(@options.fetch(:termination))
    }
  end

  def payload
    Parameters.new(params).bytes
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
