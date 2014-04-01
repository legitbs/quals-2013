package main

import (
  "net/http"
  "io"
  "io/ioutil"
  "encoding/hex"
  "pat"
  "log"
  "views"
  "dimebag"
)

var port = ":8042"

func Index(w http.ResponseWriter, req *http.Request) {
  views.WriteIndex(w)
}


func Login(w http.ResponseWriter, req *http.Request) {
  username := req.FormValue("username")

  if username == "admin" {
    views.WriteIndexFlash(w, "admin login disabled")
    return
  }
  encryptedUsername := []byte(dimebag.Encrypt(username))
  hexUsername := hex.EncodeToString(encryptedUsername)

  cookie := &http.Cookie{
    Name: "username", 
    Value: hexUsername, 
    }
  log.Printf("cookie %s", cookie.String())
  http.SetCookie(w, cookie)

  http.Redirect(w, req, "/welcome", 303)
}

func Welcome(w http.ResponseWriter, req *http.Request) {
  cookie, err := req.Cookie("username")
  if err != nil {
    io.WriteString(w, "no cookie, get lost")
    return
  }

  dehexedUsername, err := hex.DecodeString(cookie.Value)
  if err != nil {
    io.WriteString(w, "mangled cookie, it should be hex, get lost")
  }
  decryptedUsername := dimebag.Encrypt(string(dehexedUsername))

  flag := "only for the admin"

  if (decryptedUsername == "admin") {
    flagtmp, err := ioutil.ReadFile("/home/badmedicine/flag")
    if err != nil { panic(err) }
    flag = string(flagtmp)
  }

  views.WriteSuccess(w, decryptedUsername, flag)
}

func main() {
  m := pat.New()
  m.Get("/", http.HandlerFunc(Index))
  m.Post("/login", http.HandlerFunc(Login))
  m.Get("/welcome", http.HandlerFunc(Welcome))

  http.Handle("/", m)
  log.Printf("Listening on %s", port)
  err := http.ListenAndServe(port, nil)
  if err != nil {
    log.Fatal("ListenAndServe: ", err)
  }
}
