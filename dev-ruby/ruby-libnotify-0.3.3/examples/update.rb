#!/usr/bin/env ruby

=begin

update.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify update example

=end

require 'RNotify'

Notify.init( "Test2" )

test = Notify::Notification.new( "Test 2", "This is a test", nil, nil )
test.timeout= Notify::Notification::EXPIRES_NEVER
test.show

sleep(3)

test.update( "Test 2", "Message replaced!", nil )
test.timeout=5000       #5 seconds
test.show

sleep(5)

test.close
Notify.uninit
