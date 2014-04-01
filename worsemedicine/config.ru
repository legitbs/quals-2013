require 'cgi'
require 'sinatra'
require 'sinatra/cookies'
require 'securerandom'
require 'openssl'
require 'active_support/core_ext/object/to_param'

require './web'

run Sinatra::Application
