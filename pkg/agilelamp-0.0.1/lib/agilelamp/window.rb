module AgileLamp
  class Window
    def setup_light
      @light_image = Gtk::Image.new
      @green_light= Gdk::Pixbuf.new(File.join(Config.datadir("agilelamp"), "green_light.png"))
      @red_light = Gdk::Pixbuf.new(File.join(Config.datadir("agilelamp"), "red_light.png"))
      @light_image.pixbuf = @green_light
      @light_state = 'green'
      event_box = Gtk::EventBox.new.add(@light_image)
      @vbox.add(event_box)
      tooltip = Gtk::Tooltips.new
      tooltip.set_tip(event_box, "Go ahead, click it!", nil)
      event_box.signal_connect('button_press_event') {  toggle_light }
    end

    def setup_treeview
      @vbox.add(@scrolley = Gtk::ScrolledWindow.new)
      @scrolley.add(tview = Gtk::TreeView.new)
      tview.model = store = Gtk::ListStore.new(String, String, String) 
      tview.show
      renderer = Gtk::CellRendererText.new
      col1 = Gtk::TreeViewColumn.new("Build Event", renderer, "text", 0)
      col2 = Gtk::TreeViewColumn.new("Caused By", renderer, "text", 1)
      col3 = Gtk::TreeViewColumn.new("Message", renderer, "text", 2)
      tview.append_column(col1)
      tview.append_column(col2)
      tview.append_column(col3)

      iter = store.append 
      iter[0] = "Failed!"
      iter[1] = "Bob"
      iter[2] = "The thing broke. Damn you, Bob!"
      tview.show_all
    end

    def initialize
      @win = Gtk::Window.new("AgileLamp")
      @win.add(@vbox = Gtk::VBox.new)
      @vbox.add(menubar = Gtk::MenuBar.new)
      menubar.append(menu = Gtk::MenuItem.new("_Options"))
      menubar.append(menu2 = Gtk::MenuItem.new("_Tasks"))
      menubar.append(menu3 = Gtk::MenuItem.new("_Help"))
      menu.submenu = file_submenu = Gtk::Menu.new 
      menu2.submenu = tasks_submenu = Gtk::Menu.new 
      menu3.submenu = help_submenu = Gtk::Menu.new 
      file_submenu.add(Gtk::MenuItem.new("Blah"))
      tasks_submenu.add(Gtk::MenuItem.new("Blah"))
      help_submenu.add(about_submenu = Gtk::MenuItem.new("_About"))
      @vbox.add(Gtk::Toolbar.new)
      setup_light
      setup_treeview 
      setup_gconf_observation
      @vbox.show_all
      @win.show_all
    end

    def break_build
      @client["/apps/agilelamp/broken_build"] = true
      puts "broke build"
    end

    def fix_build
      @client["/apps/agilelamp/broken_build"] = false
    end

    def setup_gconf_observation
      @client = GConf::Client.default
      puts "Setting up observation of broken_build"
      @client.add_dir("/apps/agilelamp")
      @client.notify_add("/apps/agilelamp/broken_build") do |client, entry|
        puts "broken_build value changed to #{entry.value.inspect}"
        @light_image.pixbuf = entry.value == true ?  @red_light : @green_light
      end
      puts "Should have registered broken_build"
    end

    def toggle_light
      @light_image.pixbuf = @light_state == 'red' ? @red_light : @green_light
      @light_state = @light_state == 'red' ? 'red' : 'green'
      @light_state == 'red' ? break_build : fix_build 
    end

    def signal_connect signal
      @win.signal_connect signal do
        yield
      end
    end
  end
end
