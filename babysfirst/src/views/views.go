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

func WriteIndex(w io.Writer) {
  index.Execute(w, map[string]interface{}{})
}

func WriteSuccess(w io.Writer, username string) {
  model := map[string]interface{}{
    "username": username,
  }
  success.Execute(w, model)
}
