task :test do
  sh 'bundle exec rspec --require spec_helper', verbose: false
end

task :lint do
  sh 'cd spec && bundle exec rubocop -c .rubocop.yml', verbose: false
end
