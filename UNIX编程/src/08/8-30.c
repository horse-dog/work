/**************************************************
 * @file 8-30.c
 * @author your name (you@domain.com)
 * @brief 更改 nice 值的效果
 * @note usage: taskset -c 1 ./a.out 2
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
__uint64_t count;
struct timeval end;

void checktime(char *str)
{ struct timeval tv;
  gettimeofday(&tv, NULL);
  if (tv.tv_sec >= end.tv_sec && tv.tv_usec >= end.tv_usec)
  { printf("%s count = %lu\n", str, count); exit(0); }
}

/**************************************
 * @note usage: taskset -c 1 ./a.out 2
 * @param argc 
 * @param argv 
 * @return int 
 **************************************/
int main(int argc, char *argv[])
{ __pid_t pid;
  char   *s;
  int     nzero, ret;
  int     adj = 0;
  setbuf(stdout, NULL); /* no buffer stdout */
  nzero = sysconf(_SC_NZERO);
  printf("NZERO = %d\n", nzero);
  if (argc == 2) adj = atol(argv[1]);
  gettimeofday(&end, NULL);
  end.tv_sec += 10; /* run for 10 seconds */
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork failed\n"); return 1; }
  else if (pid == 0) /* child */
  { s = "child";
    printf("current nice value in child is %d, adjusting by %d\n", nice(0) + nzero, adj);
    errno = 0;
    if ((ret = nice(adj)) == -1 && errno != 0)
    { fprintf(stderr, "child set scheduling priority\n"); _exit(1); }
    printf("now child nice value is %d\n", ret + nzero);
  } 
  else  /* parent */
  { s = "parent";
    printf("current nice value in parent is %d\n", nice(0) + nzero);
  }
  for (;;)
  { if (++count == 0) 
    { fprintf(stderr, "%s counter wrap", s); return -1; }
    checktime(s);
  }
  return 0;
}