module AgileLamp
  require 'gconf2'
  require 'observer'

  class GConfObserver

    include Observable

    def initialize
      @client = GConf::Client.default
      @client["/apps/agilelamp/build_state"] = "false"
      observe_gconf
    end

    def observe_gconf
      @client.add_dir("/apps/agilelamp")
      @client.notify_add("/apps/agilelamp/build_state") do |client, entry|
        puts "broken_build value changed"
        changed
        notify_observers(client, entry)
      end
    end

    def install_schemas
      schema_file = File.join(Config.datadir("agilelamp"), "agilelamp.schemas")
      `gconftool-2 --install-schema-file #{schema_file}`
    end

    def build_state
      @client["/apps/agilelamp/build_state"]
    end

    def build_state=(value)
      @client["/apps/agilelamp/build_state"] = value
    end

    def website
      @client["/apps/agilelamp/website"]
    end

    def website=(value)
      @client["/apps/agilelamp/website"] = value
    end

    def username
      @client["/apps/agilelamp/username"]
    end

    def username=(value)
      @client["/apps/agilelamp/username"] = value
    end

    def password
      @client["/apps/agilelamp/password"]
    end

    def password=(value)
      @client["/apps/agilelamp/password"] = value
    end

    def check_interval
      @client["/apps/agilelamp/check_interval"]
    end

    def check_interval=(value)
      @client["/apps/agilelamp/check_interval"] = value
    end
  end
end
