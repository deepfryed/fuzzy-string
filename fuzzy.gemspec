# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{fuzzy}
  s.version = "0.1.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Bharanee Rathna"]
  s.date = %q{2012-02-15}
  s.description = %q{A collection of functions for fuzzy string matching.}
  s.email = ["deepfryed@gmail.com"]
  s.extensions = ["ext/fuzzy/extconf.rb"]
  s.files = ["ext/fuzzy/fuzzy.c", "ext/fuzzy/version.h", "ext/fuzzy/extconf.rb", "test/helper.rb", "test/test_fuzzy.rb", "lib/fuzzy.rb", "README.md", "CHANGELOG"]
  s.homepage = %q{http://github.com/deepfryed/fuzzy}
  s.require_paths = ["lib"]
  s.rubygems_version = %q{1.3.7}
  s.summary = %q{Fuzzy string matching}

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<rake>, [">= 0"])
    else
      s.add_dependency(%q<rake>, [">= 0"])
    end
  else
    s.add_dependency(%q<rake>, [">= 0"])
  end
end
