#!/usr/bin/env ruby

=begin

info.rb

Copyright (LGPL) 2006 Luca Russo
vargolo@gmail.com

ruby-libnotify info example

=end

require 'RNotify'

Notify.init( "Test8" )

puts "APPLICATION NAME: #{Notify.app_name}\n"
puts "SERVER CAPS:      #{Notify.server_caps}\n"
puts "SERVER INFO:      #{Notify.server_info}\n"

sleep(2)
Notify.uninit
