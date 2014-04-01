package phial

import (
  "log"
//  "bytes"
//  "io/ioutil"
  "database/sql"
  _ "go-sqlite3"
  "fmt"
)

var db *sql.DB

func init() {
  mydb, err := sql.Open("sqlite3", "/home/babysfirst/db")
  if err != nil {
    log.Fatal("sql.Open: ", err)
  }
  db = mydb

//  sqlBytes, err := ioutil.ReadFile("init.sql")
//  if err != nil {
//    log.Fatal("ioutil.ReadFile: ", err)
//  }
//
//  sqlBuf := bytes.NewBuffer(sqlBytes)
//  
//  for {
//    stmt, err := sqlBuf.ReadString(';')
//    if err != nil {
//      break
//    }
//    db.Exec(stmt)
//  }
}

func GetKey() string {

  row := db.QueryRow("select value from keys;")
  var key string
  err := row.Scan(&key)
  if err != nil {
    log.Fatal("row.Scan: ", err)
  }
  return key
}

func Login(username string, password string) (interface{}, string) {
  query := fmt.Sprintf(
    "select name from users where name = '%s' and password = '%s' limit 1;",
    username, password)
  log.Print(query)
  row := db.QueryRow(query)

  var foundUsername string
  err := row.Scan(&foundUsername)
  if err != nil {
    return nil, query
  }
  return foundUsername, query
}
