#!/usr/bin/env ruby

=begin

action.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify action example

=end

require 'RNotify'

Gtk.init
Notify.init( "Test3" )

test = Notify::Notification.new( "Test 3", "action test", nil, nil )
test.timeout= Notify::Notification::EXPIRES_NEVER

test.add_action( "Click Me action", "Click Me!", "bye!!" ) do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
