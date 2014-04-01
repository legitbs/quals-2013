#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include <string.h>

#include "../common/sharedfuncs.h"

struct _llist;

int broken;

typedef struct _llist {
  struct _llist *next;
  uint32_t tag;
  char data[100];
} llist;

FILE *randFile = NULL;

int max_len = 16;

llist *addNodes(llist *head, uint32_t tag, char * data, uint32_t num);

int ff(int connfd);

int goodRand() {
  FILE * devrand;
  uint32_t ret;
  devrand = fopen("/dev/urandom", "r");
  fread(&ret, 4, 1, devrand);
  fclose(devrand);
  return ret;
}

int main(int argc, char ** argv)
{
  int sockfd = SetupSock( 22222, AF_INET, "eth0" );
  accept_loop( sockfd, ff, "link" );
  return 0;
}

int ff(int connfd)
{
  llist *head;
  llist *cur;
  head = calloc(sizeof(llist), 1);
  FILE *key;
  char keydata[100];
  int i;
  i = 0;
  int temp;

  temp = goodRand() & 0xffff;
  cur = addNodes(head, goodRand(), NULL, temp);
  key = fopen("/home/link/key", "r");
  memset(keydata, 0, 100);
  strcpy(keydata, "The key is: ");
  fread((keydata+strlen("The key is: ")), 100-strlen("The key is: "), 1, key);
  cur = addNodes(cur, 0x41414100, keydata, 1);
  memset(keydata, 0, 100);
  fclose(key);
  temp = goodRand() & 0xffff;
  addNodes(cur, goodRand(), NULL, temp);
  cur = head;
  i = 0;

  char *userBuf;
  userBuf = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);
  char tmp[128];
  snprintf(tmp, 128, "List built.  Send me your shellcode.  Max size: %d\n", max_len);
  send_string(connfd, tmp);
  recv_stream(connfd, userBuf, max_len);
  shutdown(connfd, SHUT_RD);
  broken = connfd;
  register char *lol;
  char *(*func)();
  func = (char *(*)(llist *))userBuf;
  lol = (char *)(*func)(head);
  send_string(broken, lol);
  exit(0);
}

llist *addNodes(llist *head, uint32_t tag, char * data, uint32_t num) {
  llist *cur;
  llist *add;

  add = calloc(sizeof(llist), num);
  if(randFile == NULL)
    randFile = fopen("/dev/urandom", "r");
  fread(add, sizeof(llist), num, randFile);
  cur = head;
  int i =0;
  while(i < num)
  {
    cur->next = add;
	if(data) {
	    memcpy(add->data, data, 100);
		add->tag = tag;
	}
    cur = cur->next;
    add++;
    i++;
  }
  if(randFile)
	  fclose(randFile);
  randFile = NULL;
  return cur;
}
