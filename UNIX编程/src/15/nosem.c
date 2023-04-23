#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char message = 'X';
  int i = 0;

  if (argc > 1) {
    message = argv[1][0];
  }

  for (i = 0; i < 10; ++i) {
    printf("%c", message);
    fflush(stdout);
    sleep(rand() % 3);
    printf("%c", message);
    fflush(stdout);
    sleep(rand() % 3);
  }

  sleep(10);
  printf("\n%d - finished\n", getpid());

  exit(EXIT_SUCCESS);
}