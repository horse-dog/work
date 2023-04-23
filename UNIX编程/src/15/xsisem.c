#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *arry;
};

static int sem_id = 0;

int main(int argc, char *argv[]) {
  key_t akey = 1000;
  char message = 'X';
  int i = 0;

  // create semaphore
  sem_id = semget(akey, 1, 0666 | IPC_CREAT);
  if (-1 == sem_id) {
    fprintf(stderr, "Failed to create semaphore\n");
    exit(EXIT_FAILURE);
  }

  if (argc > 1) {
    // semaphore initialization, must
    union semun sem_union;
    sem_union.val = 1;
    if (-1 == semctl(sem_id, 0, SETVAL, sem_union)) {
      fprintf(stderr, "Failed to initialize semaphore\n");
      exit(EXIT_FAILURE);
    }

    message = argv[1][0];
    sleep(1);
  }

  for (i = 0; i < 10; ++i) {
    // go into critical zone
    struct sembuf sem_i;
    sem_i.sem_num = 0;
    sem_i.sem_op = -1;
    sem_i.sem_flg = SEM_UNDO;
    if (-1 == semop(sem_id, &sem_i, 1)) {
      perror("semop in failed\n");
      exit(EXIT_FAILURE);
    }

    printf("%c", message);
    fflush(stdout);
    sleep(rand() % 3);
    printf("%c", message);
    fflush(stdout);

    // leave critical zone
    struct sembuf sem_o;
    sem_o.sem_num = 0;
    sem_o.sem_op = 1;
    sem_o.sem_flg = SEM_UNDO;
    if (-1 == semop(sem_id, &sem_o, 1)) {
      perror("semop out failed\n");
      exit(EXIT_FAILURE);
    }

    sleep(rand() % 3);
  }

  sleep(10);
  printf("\n%d - finished\n", getpid());

  if (argc > 1) {
    // delete samaphore
    sleep(3);
    union semun sem_union;
    if (-1 == semctl(sem_id, 0, IPC_RMID, sem_union)) {
      fprintf(stderr, "Failed to delete semaphore\n");
    }
  }

  exit(EXIT_SUCCESS);
}