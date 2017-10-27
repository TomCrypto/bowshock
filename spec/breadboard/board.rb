# Links expected:
#   device => program upload link to device
#   main => serial link to device UART0

class Breadboard
  def initialize(options, links)
    @options = options
    @links = links
  end

  def upload(program)
    @links[:device].upload program
  end

  def response
    @response ||= Drivers::JSON.new(@links[:main], payload).run
  end

  private

  def payload
    Class.new BinaryStruct do
      layout :termination, :uint
    end.new(params).bytes
  end

  def params
    @params ||= {
      termination: TERMINATIONS.fetch(@options.fetch(:termination))
    }
  end

  TERMINATIONS = {
    floating: 0 << 3,
    pulldown: 1 << 3,
    pullup: 2 << 3,
    repeater: 3 << 3
  }.freeze
end
