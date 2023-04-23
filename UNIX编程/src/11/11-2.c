/**************************************************
 * @file 11-2.c
 * @author your name (you@domain.com)
 * @brief 打印线程ID
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
pthread_t ntid;
void printids(const char *s)
{ pid_t     pid = getpid();
  pthread_t tid = pthread_self();
  printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
          (unsigned long)tid, (unsigned long)tid);
}

void *thr_fn(void *arg)
{ printids("new thread: ");
  return ((void*)0);
}

int main(int argc, char *argv[])
{ int err = pthread_create(&ntid, NULL, thr_fn, NULL);
  if (err != 0)
  { fprintf(stderr, "can't create thread\n"); return 1; }
  printids("main thread: ");
  sleep(1);
  return 0;
}