/**************************************************
 * @file 11-5.c
 * @author your name (you@domain.com)
 * @brief 线程清理处理程序
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <pthread.h>

void cleanup(void *arg)
{ printf("clean up: %s\n", (char *)arg); }

void *thr_fn1(void *arg)
{ printf("thread 1 start\n");
  pthread_cleanup_push(cleanup, "thread 1 first handler");
  pthread_cleanup_push(cleanup, "thread 1 second handler");
  printf("thread 1 push complete\n");
  if (arg) return ((void*)1);
  pthread_cleanup_pop(0);
  pthread_cleanup_pop(0);
  return ((void*)1);
}

void *thr_fn2(void *arg)
{ printf("thread 2 start\n");
  pthread_cleanup_push(cleanup, "thread 2 first handler");
  pthread_cleanup_push(cleanup, "thread 2 second handler");
  printf("thread 2 push complete\n");
  if (arg) pthread_exit((void*)2);
  pthread_cleanup_pop(0);
  pthread_cleanup_pop(0);
  pthread_exit((void*)2);
}

int main(int argc, char *argv[])
{ pthread_t tid1, tid2;
  void           *tret;
  pthread_create(&tid1, NULL, thr_fn1, (void*)1);
  pthread_create(&tid2, NULL, thr_fn2, (void*)1);
  pthread_join(tid1, &tret);
  printf("thread 1 exit code %ld\n", (long)tret);
  pthread_join(tid2, &tret);
  printf("thread 2 exit code %ld\n", (long)tret);
  return 0;
}