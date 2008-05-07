module AgileLamp
  require 'rubygems'
  require 'mechanize'
  require 'rss/1.0'
  require 'rss/2.0'

  class CruiseParser
    def initialize server, user="", password=""
      @agent = WWW::Mechanize.new
      @agent.basic_auth user, password
      @url = "http://#{server}/projects/findrate?format=rss"
    end

    def check_server
      begin
        page = @agent.get @url
        rss = RSS::Parser.parse(page.body, false) 
        result = rss.items.first.title.match(/failed/) ? false : true
      rescue
        puts "failed to get response from server"
        :server_failure
      end
    end
  end
end

if __FILE__ == $0
  parser = AgileLamp::CruiseParser.new "cruise.meihome.com", "method", "mysticar" 
  5.times do
    puts parser.check_server
  end
end
