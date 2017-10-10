def deep_symbolize_keys(object)
  case object
  when Hash
    object.each_with_object({}) do |(key, value), result|
      result[key.to_sym] = deep_symbolize_keys value
    end
  when Array
    object.map(&method(:deep_symbolize_keys))
  else
    object
  end
end

def make_link(type:, args: {})
  case type
  when 'lpc21isp'
    Link::LPC21ISP.new(**args)
  when 'serial'
    Link::Serial.new(**args)
  else
    raise "unknown link type #{type}"
  end
end

def load_links(file)
  configs = deep_symbolize_keys YAML.safe_load File.read file
  Hash.new { |hash, key| hash[key] = make_link configs[key] }
rescue Errno::ENOENT
  nil
end

RSpec.shared_context 'Hardware Links', hardware: true do
  let(:location) { |example| example.metadata[:absolute_file_path] }
  let(:links) { load_links "#{File.dirname location}/board_links.yml" }

  before { skip 'links not configured' if links.nil? }
  after { links.values.each(&:stop) unless links.nil? }
end
