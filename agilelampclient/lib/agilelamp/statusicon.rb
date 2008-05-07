module AgileLamp
  class StatusIcon
    def initialize
      @status = Gtk::StatusIcon.new
      @status.stock = Gtk::Stock::ADD
      @status.tooltip = "What should you be doing?" 
      @status.signal_connect('activate') { on_activate }
      @status.signal_connect('popup-menu') {|statusicon, button, time| on_right_click statusicon, button, time }
      @menu = Gtk::Menu.new
      @menu.append(preferences = Gtk::ImageMenuItem.new(Gtk::Stock::PREFERENCES))
      @menu.append(about = Gtk::ImageMenuItem.new(Gtk::Stock::ABOUT))
      @menu.append(Gtk::SeparatorMenuItem.new)
      @menu.append(quit = Gtk::ImageMenuItem.new(Gtk::Stock::QUIT))
      preferences.signal_connect('activate') { on_click_preferences }
      about.signal_connect('activate') { on_click_about }
      quit.signal_connect('activate') { Gtk.main_quit }

      @gconf = GConfObserver.new
      @gconf.install_schemas
      @gconf.add_observer(self)
      observe_cruisecontrol
    end

    def update(*arg)
      client, entry = arg[0], arg[1]
      puts "StatusIcon:"
      puts client, entry
      entry.value == true ? broken_notification : fixed_notification
      @status.stock = entry.value == true ? Gtk::Stock::CANCEL : Gtk::Stock::APPLY
    end

    def broken_notification
      `notify-send -u critical "Something is broken." "Blah broke on server Blah."`
    end

    def fixed_notification
      `notify-send -u normal "Something is fixed." "Issue Blah is fixed on server Blah."`
    end

    def break_build
      @gconf.build_state = "broken"
      puts "broke build"
    end

    def fix_build
      @gconf.build_state = "fixed"
    end

    def check_server
      result = @cruise.check_server 
      puts "Server was #{result}"
      puts "Next poll in #{@gconf.check_interval} minutes."
      case result
      when true
        fix_build
      when false
        break_build
      when :server_failure
        @status.stock == Gtk::Stock::DIALOG_WARNING
      end
    end

    def observe_cruisecontrol
      @cruise ||= CruiseParser.new(@gconf.website, @gconf.username, @gconf.password)

      # Do it one time to start
      check_server 

      Gtk.timeout_add(6000 * @gconf.check_interval) do
        check_server 
        true 
      end
    end

    def on_click_preferences
      PreferencesWindow.new("Preferences").show_all
    end

    def on_activate
      @window ||= Proc.new { window = Window.new; window.signal_connect('destroy') { @window = nil }; puts "#{@window}"; window }.call
    end

    def on_click_about
      Gtk::AboutDialog.new.show_all
    end
    def on_right_click(statusicon, button, time)
      @menu.popup(nil, nil, button, time) {|menu, x, y, push_in| @status.position_menu(@menu)}
      @menu.show_all
    end
  end
end
