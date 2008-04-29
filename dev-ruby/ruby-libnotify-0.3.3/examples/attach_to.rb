#!/usr/bin/env ruby

=begin

attach_to.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify attach_to example

=end

require 'RNotify'

Gtk.init
Notify.init( "Test9" )

win = Gtk::Window.new( Gtk::Window::TOPLEVEL )
win.signal_connect( "destroy" ) { Gtk.main_quit }

bt = Gtk::Button.new( "Click Me!" )

win.add( bt )
win.show_all

test = Notify::Notification.new( "Attach_To", "Do you savvy? :P", nil, bt )
test.timeout= Notify::Notification::EXPIRES_NEVER
test.add_action( "Attach_To", "Click Me!" ) { Gtk.main_quit }

bt.signal_connect( "clicked", test ) do |wid,data|
  data.show
end

Gtk.main

test.clear_actions
test.close
Notify.uninit
