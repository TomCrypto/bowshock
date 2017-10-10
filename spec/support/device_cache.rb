# This simple global cache exists for the sole purpose of preventing redundant
# device uploads, which would otherwise quickly wear down their flash memory.
module DeviceCache
  @cache = {}

  class << self
    def upload_program(device, program)
      yield unless @cache[[device, program]]
      @cache.store [device, program], true
    end
  end
end
