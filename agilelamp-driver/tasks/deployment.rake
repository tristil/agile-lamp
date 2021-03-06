desc 'Release the website and new gem version'
task :deploy => [:check_version, :website, :release] do
  puts "Remember to create SVN tag:"
  puts "svn copy svn+ssh://#{rubyforge_username}@rubyforge.org/var/svn/#{PATH}/trunk " +
    "svn+ssh://#{rubyforge_username}@rubyforge.org/var/svn/#{PATH}/tags/REL-#{VERS} "
    puts "Suggested comment:"
    puts "Tagging release #{CHANGES}"
end

desc 'Runs tasks website_generate and install_gem as a local deployment of the gem'
task :local_deploy => [:website_generate, :install_gem]

task :check_version do
  unless ENV['VERSION']
    puts 'Must pass a VERSION=x.y.z release version'
    exit
  end
  unless ENV['VERSION'] == VERS
    puts "Please update your version.rb to match the release version, currently #{VERS}"
    exit
  end
end

desc 'Install the package as a gem, without generating documentation(ri/rdoc)'
task :install_gem_no_doc => [:clean, :package] do
  sh "#{'sudo ' unless Hoe::WINDOZE }gem install pkg/*.gem --no-rdoc --no-ri"
end

#task :make_agilelamp_bin do
#  cd "src/"
#  begin
#    sh "make"
#    cp "agilelamp-driver", "../bin"
#  rescue
#    puts <<EOF 
#
#******************************************************************************
#Couldn't compile for your architecture. Precompiled binary may still work.  If
#not, try installing libusb headers (compile and install libusb from source or
#get libusb-dev on debian systems)." 
#******************************************************************************
#
#EOF
#  end
#  cd "../"
#end

#task :install_udev_rule do
#  begin
#    if `uname`.strip == 'Linux'
#      puts "Installing udev rule to enable hardware use on Linux"
#      cp "src/44-usblamp.rules", "/etc/udev/rules.d/"
#      puts "Restarting udev..."
#      sh "/etc/init.d/udev restart"
#    end
#  rescue
#    puts <<EOF

#****************************************************************************
#Couldn't install udev rules for the USB lamp. Expected to be able to put file
#in /etc/udev/rules.d/. Was this a bad assumption?
#****************************************************************************

#EOF
#  end
#end

#task :install_gem => [:clean, :make_agilelamp_bin, :package, :install_udev_rule, ] do
#  sh "#{'sudo ' unless Hoe::WINDOZE}gem install --no-wrappers --local pkg/*.gem"
#end

namespace :manifest do
  desc 'Recreate Manifest.txt to include ALL files'
  task :refresh do
    `rake check_manifest | patch -p0 > Manifest.txt`
  end
end
