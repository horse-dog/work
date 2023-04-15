#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds* buf;
  unsigned short int* array;
  struct seminfo* __buf;
};

// op为-1执行p操作，op为1时执行v操作
void pv(int sem_id, int op) {
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = op;
  sem_b.sem_flg = SEM_UNDO;
  semop(sem_id, &sem_b, 1);
}

void p(int sem_id) {
  pv(sem_id, -1);
}

void v(int sem_id) {
  pv(sem_id, 1);
}

int main(int argc, char* argv[]) {

  int sem_id = semget(IPC_PRIVATE, 1, 0666);
  union semun sem_un;
  sem_un.val = 1;
  semctl(sem_id, 0, SETVAL, sem_un);

  pid_t id = fork();
  if (id < 0) return 1;
  else if (id == 0) {
    printf("child try to get binary sem\n");
    p(sem_id);
    printf("child get the sem and would release it afer 5 seconds\n");
    sleep(5);
    v(sem_id);
    exit(0);
  } else {
    printf("parent try to get binary sem\n");
    p(sem_id);
    printf("parent get the sem and would release it after 5 seconds\n");
    sleep(5);
    v(sem_id);
  }

  waitpid(id, NULL, 0);
  semctl(sem_id, 0, IPC_RMID, sem_un); // 删除信号量
  return 0;

}