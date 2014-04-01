package main

import (
  "net/http"
  "pat"
  "log"
  "phial"
  "views"
)

var port = ":8041"

func Index(w http.ResponseWriter, req *http.Request) {
  views.WriteIndex(w)
}


func Login(w http.ResponseWriter, req *http.Request) {
  username := req.FormValue("username")
  password := req.FormValue("password")
  foundUsername, sql := phial.Login(username, password)

  w.Header().Set("X-SQL", sql)

  switch u := foundUsername.(type) {
  case string:
    w.WriteHeader(200)
    views.WriteSuccess(w, u)
    return
  case nil:
    w.WriteHeader(403)
    views.WriteIndex(w)
    return
  }
}

func main() {
  m := pat.New()
  m.Get("/", http.HandlerFunc(Index))
  m.Post("/login", http.HandlerFunc(Login))

  http.Handle("/", m)
  log.Printf("Listening on %s", port)
  err := http.ListenAndServe(port, nil)
  if err != nil {
    log.Fatal("ListenAndServe: ", err)
  }
}
