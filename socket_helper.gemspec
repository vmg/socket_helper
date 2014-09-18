Gem::Specification.new do |s|
  s.name = 'socket_helper'
  s.version = '0.1.0'
  s.authors = ["Vicent Marti"]
  s.date = Time.now.utc.strftime("%Y-%m-%d")
  s.email = %q{tanoku@gmail.com}
  s.extensions = ["ext/socket_helper/extconf.rb"]
  s.files = `git ls-files`.split("\n")
  s.homepage = %q{http://github.com/vmg/socket_helper}
  s.rdoc_options = ["--charset=UTF-8"]
  s.require_paths = ["lib"]
  s.rubygems_version = %q{1.4.2}
  s.summary = %q{A thing for sockets}

  # tests
  s.add_development_dependency 'rake-compiler', ">= 0.7.5"
end
