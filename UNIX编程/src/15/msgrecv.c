#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

struct msg_st {
  long int msg_type;
  char text[BUFSIZ];
};

int main(void) {
  struct msg_st data;
  data.msg_type = 0;
  key_t akey = 1000;
  int msgid = -1;
  bool running = true;

  // create messge queue
  msgid = msgget(akey, 0666 | IPC_CREAT);
  if (-1 == msgid) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  // loop for getting data from message queue
  while (running) {
    // receive data
    if (-1 == msgrcv(msgid, (void*)&data, BUFSIZ, data.msg_type, 0)) {
      fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
      exit(EXIT_FAILURE);
    }
    printf("Receive text: %s", data.text);
    // receive "quit" to finish
    if (0 == strncmp(data.text, "quit", 4)) {
      running = false;
    }
  }

  // delete message queue
  if (-1 == msgctl(msgid, IPC_RMID, 0)) {
    fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}