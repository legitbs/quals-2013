package dimebag

import (
  "io/ioutil"
  "log"
  "crypto/cipher"
  "crypto/aes"
)

var key []byte
var iv []byte

func init() {
  myKey, err := ioutil.ReadFile("key")
  if err != nil {
    log.Fatal("dimebag read key: ", err)
  }

  myIv, err := ioutil.ReadFile("iv")
  if err != nil {
    log.Fatal("dimebag read iv: ", err)
  }

  key = myKey
  iv = myIv

  log.Printf("key %x", key)
  log.Printf("iv %x", iv)
  log.Printf("expecting %x", Encrypt("admin"))
}

func Encrypt(username string) string {
  cipher := ctr()
  inBytes := []byte(username)
  outBytes := make([]byte, len(inBytes))

  cipher.XORKeyStream(outBytes, inBytes)

  return string(outBytes)
}

func block() cipher.Block {
  block, err := aes.NewCipher(key)
  if err != nil {
    log.Fatal("couldn't create aes")
  }
  return block
}

func ctr() cipher.Stream {
  encryptor := cipher.NewCTR(block(), iv)
  return encryptor
}
