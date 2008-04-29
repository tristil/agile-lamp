#!/usr/bin/env ruby

=begin

geometry-hints.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify geometry-hints example

=end

require 'RNotify'

Gtk.init
Notify.init( "Test10" )

screen = Gdk::Screen.default

test = Notify::Notification.new( "Test10", "This notification point to 150,100", nil, nil )
test.geometry_hints( screen, 150, 100 )
test.timeout= Notify::Notification::EXPIRES_NEVER
test.add_action( "Action", "Click Me!" ) { Gtk.main_quit }
test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
