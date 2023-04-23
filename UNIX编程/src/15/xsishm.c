#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SIZE 1024
#define exit_err(str)   \
  do {                  \
    perror(str);        \
    exit(EXIT_FAILURE); \
  } while (0);
#define uint32 unsigned long

int main(void) {
  int shmid;
  char *shmptr;
  key_t key;
  pid_t pid;

  if ((pid = fork()) < 0) {
    exit_err("fork error");
  }

  if (0 == pid) {
    printf("child process\n");

    if ((key = ftok("/dev/null", O_RDWR)) < 0)
      exit_err("ftok error");

    if ((shmid = shmget(key, SIZE, 0600 | IPC_CREAT)) < 0)
      exit_err("shmget error");

    if ((shmptr = (char *)shmat(shmid, 0, 0)) == (void *)-1)
      exit_err("shmat error");

    sleep(1);
    printf("child pid is %d, share memory from %lx to %lx, content: %s\n",
           getpid(), (uint32)shmptr, (uint32)(shmptr + SIZE), shmptr);
    sleep(1);

    if ((shmctl(shmid, IPC_RMID, 0) < 0)) {
      exit_err("shmctl error");
    }
    exit(EXIT_SUCCESS);
  } else {
    printf("parent process\n");

    if ((key = ftok("/dev/null", O_RDWR)) < 0) {
      exit_err("ftok error");
    }

    if ((shmid = shmget(key, SIZE, 0600 | IPC_CREAT | IPC_EXCL)) < 0) {
      exit_err("shmget error");
    }

    if ((shmptr = (char *)shmat(shmid, 0, 0)) == (void *)-1) {
      exit_err("shmat error");
    }

    memcpy(shmptr, "hello world", sizeof("hello world"));
    printf("parent pid is %d, share memory from %lx to %lx, content: %s\n",
           getpid(), (uint32)shmptr, (uint32)(shmptr + SIZE), shmptr);
  }

  waitpid(pid, NULL, 0);
  exit(EXIT_SUCCESS);
}