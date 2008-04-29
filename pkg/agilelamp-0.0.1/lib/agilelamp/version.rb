module AgileLamp#:nodoc:
  module VERSION #:nodoc:
    MAJOR = 0
    MINOR = 0
    TINY  = 1

    STRING = [MAJOR, MINOR, TINY].join('.')
  end
  def self.version
    VERSION::STRING
  end
end
