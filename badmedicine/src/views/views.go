package views

import (
  "io"
  "log"
  "html/template"
)

var index *template.Template
var success *template.Template

func init() {
  myIndex, err := template.ParseFiles("views/index.html")
  if err != nil {
    log.Fatal("template.ParseFiles: ", err)
  }
  index = myIndex

  mySuccess, err := template.ParseFiles("views/success.html")
  if err != nil {
    log.Fatal("template.ParseFiles: ", err)
  }
  success = mySuccess
}

func WriteIndexFlash(w io.Writer, flash string) {
  model := map[string]interface{}{
    "flash": flash,
  }
  index.Execute(w, model)
}

func WriteIndex(w io.Writer) {
  WriteIndexFlash(w, "")
}

func WriteSuccess(w io.Writer, username string, key string) {
  model := map[string]interface{}{
    "username": username,
    "keymsg": key,
  }
  success.Execute(w, model)
}
