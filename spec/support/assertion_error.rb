# Represents an assertion failure on the device during a spec, which will then
# be raised by the driver and can be checked for in specs with raise_error.
class AssertionError < StandardError
  def initialize(msg, file, line)
    @file = file
    @line = line
    @msg = msg
    super msg
  end

  attr_reader :file
  attr_reader :line

  def to_s
    "#{@msg} [#{@file}:#{@line}]"
  end
end
