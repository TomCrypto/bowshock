# This is a trivial wrapper around FFI::Struct which uses a hash to initialize
# its contents, with some additional helper functions to extract its bytes.
class BinaryStruct < FFI::Struct
  def bytes
    to_ptr.read_string(size).bytes
  end

  protected

  def initialize(fields)
    fields.each { |key, value| self[key] = value }
  end
end
