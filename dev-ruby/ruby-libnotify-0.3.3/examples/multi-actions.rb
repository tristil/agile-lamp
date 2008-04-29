#!/usr/bin/env ruby

=begin

multi-actions.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify multi-actions example

=end

require 'RNotify'

Gtk.init
Notify.init( "Test4" )

test = Notify::Notification.new( "Test 4", "multi actions test", nil, nil )
test.timeout= Notify::Notification::EXPIRES_NEVER

test.add_action( "Click Me action 1", "Click Me! - 1", "bye!! - 1" ) do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.add_action( "Click Me action 2", "Click Me! - 2", "bye!! - 2" ) do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
