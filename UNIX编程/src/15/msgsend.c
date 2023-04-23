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
  data.msg_type = 1;
  char buf[BUFSIZ];
  key_t akey = 1000;
  int msgid = -1;
  bool running = true;

  // create message queue
  msgid = msgget(akey, 0666 | IPC_CREAT);
  if (-1 == msgid) {
    fprintf(stderr, "msgget failed with error: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  // loop for sending data to message queue
  while (running) {
    printf("Input text: ");
    fgets(buf, BUFSIZ, stdin);
    strcpy(data.text, buf);
    // send data
    if (-1 == msgsnd(msgid, (void*)&data, BUFSIZ, 0)) {
      fprintf(stderr, "msgsnd failed\n");
      exit(EXIT_FAILURE);
    }
    // input "quit" to finish
    if (0 == strncmp(buf, "quit", 4)) {
      running = false;
    }
  }

  exit(EXIT_SUCCESS);
}
