CRYPTO_KEY = ENV['CRYPTO_KEY'] || "\x18\x85\a\v\xFA\x9F)\0"
KEY = ENV['CTF_KEY'] || "computers downtown and computers up in harlem"

get '/' do
  haml :index
end

post '/' do
  if params[:username] == 'admin'
    raise "can't log in as admin, be smarter"
  end

  cipher = make_cipher
  cipher.encrypt
  cipher.key = CRYPTO_KEY
  iv = cipher.random_iv

  plaintext = Hash[*params.sort.flatten].to_param

  ciphertext = cipher.update plaintext
  ciphertext << cipher.final

  cookies[:iv] = iv
  cookies[:ciphertext] = ciphertext
  cookies[:verification] = params[:verification]

  redirect '/key'
end

get '/key' do
  if cookies[:ciphertext].nil? || cookies[:iv].nil?
    raise "wtf"
  end
  cipher = make_cipher
  cipher.decrypt
  cipher.key = CRYPTO_KEY
  cipher.iv = cookies[:iv]

  plaintext = cipher.update cookies[:ciphertext]
  plaintext << cipher.final

  pairs = CGI::parse plaintext

  username = pairs['username'].first || "couldn't find one"
  verification = pairs['verification'].first
  verif_right = verification == cookies[:verification]

  if username == 'admin'
    haml :win, locals: {key: KEY}
  else
    haml :fail, locals: {username: username, verif_right: verif_right}
  end
end

helpers do
  def authenticity_token
    SecureRandom.hex[0..7]
  end

  def make_cipher
    OpenSSL::Cipher::Cipher.new 'DES-CFB'
  end
end
