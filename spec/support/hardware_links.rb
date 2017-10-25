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

def links_by_name
  Links.constants.map(&Links.method(:const_get)).map do |link|
    [link::NAME, link]
  end.to_h
end

def make_link(type:, args: {})
  raise "unknown link type '#{type}'" unless links_by_name.key? type
  links_by_name[type].new(**args) # link type looked up dynamically
end

def load_links(file)
  configs = deep_symbolize_keys YAML.safe_load File.read file
  Hash.new { |hash, key| hash[key] = make_link configs[key] }
rescue Errno::ENOENT
  nil
end

LINK_FILE = 'board_links.yml'.freeze

RSpec.shared_context 'Hardware Links', hardware: true do
  let(:location) { |example| example.metadata[:absolute_file_path] }
  let(:links) { load_links "#{File.dirname location}/#{LINK_FILE}" }

  before { skip 'links not configured' if links.nil? }
  after { links.values.each(&:stop) unless links.nil? }
end
