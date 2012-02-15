require 'date'
require 'pathname'
require 'rake'
require 'rake/testtask'

$rootdir = Pathname.new(__FILE__).dirname
$gemspec = Gem::Specification.new do |s|
  s.name              = 'fuzzy-string'
  s.version           = '0'           # modify ext/version.h
  s.date              = Date.today    
  s.authors           = ['Bharanee Rathna']
  s.email             = ['deepfryed@gmail.com']
  s.summary           = 'Fuzzy string matching'
  s.description       = 'A collection of functions for fuzzy string matching.'
  s.homepage          = 'http://github.com/deepfryed/fuzzy-string'
  s.files             = Dir['ext/**/*.{c,h}'] + Dir['{ext,test,lib}/**/*.rb'] + %w(README.md CHANGELOG)
  s.extensions        = %w(ext/fuzzy-string/extconf.rb)
  s.require_paths     = %w(lib)
  s.add_development_dependency('rake')
end

desc 'Generate gemspec'
task :gemspec do 
  $gemspec.date    = Date.today
  $gemspec.version = File.read($rootdir + 'ext/fuzzy-string/version.h').scan(/[\d.]+/).first
  File.open("#{$gemspec.name}.gemspec", 'w') {|fh| fh.write($gemspec.to_ruby)}
end

Rake::TestTask.new(:test) do |test|
  test.libs   << 'ext' << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = true
end

desc 'compile'
task :compile do
  Dir.chdir('ext/fuzzy-string') do
    system('ruby extconf.rb && make clean && make -j2 > /dev/null') or raise "unable to compile extension"
  end
end

task :test => [:compile]
task default: :test
