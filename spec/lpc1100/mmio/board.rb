# Links expected:
#   device => program upload link to device
#   main => serial link to device UART0

module LPC1100
  class MMIO < JSONDriver
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
        operation: OPERATIONS.fetch(@options.fetch(:operation)),
        initial_value: @options.fetch(:initial_value),
        argument: @options.fetch(:argument) || 0
      }
    end

    def payload
      Parameters.new(params).bytes
    end

    class Parameters < BinaryStruct
      layout :operation, :uint,
             :initial_value, :uint,
             :argument, :uint
    end

    OPERATIONS = {
      masked_clear: 0,
      clear: 1,
      masked_set: 2,
      set: 3,
      toggle: 4,
      masked_write: 5,
      safe_write: 6,
      write: 7,
      read: 8,
      any: 9,
      all: 10,
      none: 11,
      clear_bit: 12,
      set_bit: 13,
      toggle_bit: 14,
      read_bit: 15
    }.freeze
  end
end
