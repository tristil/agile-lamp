#!/usr/bin/env ruby

=begin

xy.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify xy example

=end

require 'RNotify'

Notify.init( "Test7" )

test = Notify::Notification.new( "Test 7", "This notification point to 150,100", nil, nil )
test.hint32( "x", 150 )
test.hint32( "y", 100 )
test.timeout= 5000      #5 seconds
test.show

sleep(5)
test.close
Notify.uninit
