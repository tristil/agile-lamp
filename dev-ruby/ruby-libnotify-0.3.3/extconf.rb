#!/usr/bin/env ruby

require 'mkmf'

pkgconfig = with_config( "pkg-config", "pkg-config" )

$preload = nil

dir = Config::CONFIG["sitearchdir"]

def find_gtk2_rb
  if File.exist?( "#{Config::CONFIG["sitelibdir"]}/gtk2.rb" )
    puts "gtk2.rb.. yes"
  else
    if File.exist?( "#{Config::CONFIG["rubylibdir"]}/gtk2.rb" )
      puts "gtk2.rb.. yes"
    else
      return false
    end
  end
  return true
end

if find_gtk2_rb == true
  if have_library( "notify", "notify_init" )
    ver = `#{pkgconfig} libnotify --modversion`.chomp.split( /\./ )
    maj,min,rev = ver[0].to_i,ver[1].to_i,ver[2].to_i
    $CFLAGS << ' -Wall ' << "-I#{dir} " << `#{pkgconfig} libnotify --cflags`.chomp
    $LIBS << ' ' << `#{pkgconfig} libnotify --libs`.chomp
    if( maj == 0 && min >= 4 && rev >= 1 )
      $CFLAGS << ' -DHAVE_GEOMETRY_HINTS'
      puts "with geometry hints.. yes"
      ver = `#{pkgconfig} gtk+-2.0 --modversion`.chomp.split( /\./ )
      maj,min,rev = ver[0].to_i,ver[1].to_i,ver[2].to_i
      if maj == 2 and min >= 9
        $CFLAGS << ' -DHAVE_STATUS_ICON'
        puts "with GtkStatusIcon.. yes"
      end
      create_makefile( "rnotify" )
    else
      puts "with geometry hints.. no"
      puts "with GtkStatusIcon.. no"
      create_makefile( "rnotify" )
    end
  else
    puts "ERROR: no libnotify found",
    "[ http://www.galago-project.org/ ]"
  end
else
  puts "ERROR: no gtk2.rb found",
  "[ http://ruby-gnome2.sourceforge.jp/ ]"
end
