module AgileLamp
  class PreferencesWindow < Gtk::Window
    def initialize name
      super name
      @client = GConf::Client.default
      set_size_request(250, 200)
      add(vbox = Gtk::VBox.new)
      vbox.add(notebook = Gtk::Notebook.new)
      notebook.append_page(config = Gtk::VBox.new, Gtk::Label.new("Configuration"))
      notebook.append_page(testing = Gtk::VBox.new, Gtk::Label.new("Test USB Lamp"))
      config.add(Gtk::Label.new("Website for CruiseControl[.rb]"))
      config.add(hbox = Gtk::HBox.new)
      hbox.add(Gtk::Label.new("http://"))
      hbox.add(@entry = Gtk::Entry.new)
      @entry.text = @client["/apps/agilelamp/website"] or "" 
      config.add(hbox2 = Gtk::HBox.new)
      hbox2.add(Gtk::Label.new("Check every "))
      hbox2.add(@spin = Gtk::SpinButton.new(1, 60, 1))
      @spin.value = (@client["/apps/agilelamp/check_interval"] or 1)
      hbox2.add(Gtk::Label.new("minutes."))
      testing.add(hbox3 = Gtk::HBox.new)
      hbox3.add(Gtk::Label.new("Change red light: "))
      hbox3.add(red_light = Gtk::Button.new("OFF"))
      testing.add(hbox4 = Gtk::HBox.new)
      hbox4.add(Gtk::Label.new("Change green light: "))
      hbox4.add(green_light = Gtk::Button.new("OFF"))
      red_light.signal_connect('clicked') { red_light.label= red_light.label == "OFF" ? "ON" : "OFF" }
      green_light.signal_connect('clicked') {green_light.label=green_light.label == "OFF" ? "ON" : "OFF" }
      vbox.add(close_button = Gtk::Button.new(Gtk::Stock::CLOSE))
      close_button.signal_connect('clicked') { save_preferences; destroy }
    end
    def save_preferences
      @client["/apps/agilelamp/check_interval"] = @spin.value
      @client["/apps/agilelamp/website"] = @entry.text
    end
  end
end
