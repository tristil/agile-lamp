$:.unshift File.dirname(__FILE__)

module AgileLamp
  require 'gtk2'
  require 'gconf2'
  require 'singleton'
  require 'agilelamp/window'
  require 'agilelamp/statusicon'
  require 'agilelamp/preferences_window'
  require 'agilelamp/cruiseparser'
  require 'agilelamp/version'
  def self.Main
    StatusIcon.new
    Gtk.main
  end
end
