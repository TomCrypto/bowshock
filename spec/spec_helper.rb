Bundler.require :test
require_rel 'support'

RSpec.configure do |config|
  config.fail_fast = true
  config.color = true
end
