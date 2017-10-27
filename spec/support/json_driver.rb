require 'ostruct'

class AssertionError < StandardError
  def initialize(msg, file, line)
    @file = file
    @line = line
    super msg
  end

  attr_reader :file
  attr_reader :line
end

class JSONDriver
  def response
    OpenStruct.new JSON.parse output
  rescue JSON::ParserError => e
    raise parse_assertion || e
  end

  private

  def output
    @output ||= communicate
  end

  def communicate
    link.write payload
    bytes_read = []

    loop do
      byte = link.read(1).first
      break if byte.zero?
      bytes_read << byte
    end

    bytes_read.pack('C*').force_encoding('utf-8')
  end

  def parse_assertion
    match = /([^:]+):(\d+): (.*)/.match output
    return nil if match.nil? # was not an assertion
    AssertionError.new match[3], match[1].to_i, match[2]
  end
end
