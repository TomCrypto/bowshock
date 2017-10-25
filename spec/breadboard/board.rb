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

  def events
    results
  end

  private

  def protocol
    @protocol ||= Protocols::EventList.new
  end

  def device
    @links[:device]
  end

  def link
    @links[:main]
  end

  def results
    decoder = protocol.decode
    link.write protocol.encode payload

    loop do
      result = decoder.resume link.read(1).first
      break result unless decoder.alive? # done
    end
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
