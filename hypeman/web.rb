enable :sessions
set :session_secret, 'wroashsoxDiculReejLykUssyifabEdGhovHabno'

USERS = {"admin"=>false, "asdf" => 'asdf'}
Secret = Struct.new(:username, :name, :body)
SECRETS = [
  Secret.new('admin', 'key', 'watch out for this Etdeksogav'),
  Secret.new('asdf', 'nothing', 'hello')
]

USER_MUTEX = Mutex.new
SECRET_MUTEX = Mutex.new

get '/' do
  haml :index
end

post '/' do
  SECRET_MUTEX.synchronize do
    return create_user if USERS[params[:username]].nil?
    return attempt_login
  end
end

get '/secrets' do
  redirect '/' unless current_user

  haml :list
end

post '/secrets' do
  redirect '/' unless current_user

  s = Secret.new session[:user_name], params[:name], params[:body]


  SECRET_MUTEX.synchronize do
    SECRETS << s

    if SECRETS.length > 100
      keep = SECRETS[0..1]
      keep += SECRETS[80..-1]

      SECRETS = keep
    end
  end

  redirect '/secrets'
end

get '/secrets/:id' do
  s = SECRETS[params[:id].to_i]
  raise "unauthorized" if session[:user_name] != s.username

  haml :secret, locals: {secret: s}
end

helpers do
  def current_user
    return nil unless session[:user_name]
    USERS[session[:user_name]]
  end

  def create_user
    USERS[params[:username]] = params[:password]
    session[:user_name] = params[:username]
    redirect '/secrets'
  end

  def attempt_login
    redirect '/' if params[:password] != USERS[params[:username]]
      
    session[:user_name] = params[:username]
    redirect '/secrets'
  end
end
