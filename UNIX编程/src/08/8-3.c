/**************************************************
 * @file 8-3.c
 * @author your name (you@domain.com)
 * @brief vfork 函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void close_stdout()
{ fclose(stdout); }

int g_val = 6; /* external variable in initialized data */
int main(int argc, const char *argv[])
{ int val = 1; /* automatic variable on the stack */
  pid_t pid;
  if (atexit(close_stdout))
  { fprintf(stderr, "can't regist colse_stdout\n");
    return -1;
  }
  printf("before vfork\n");/* we don't flush stdio */
  if ((pid = vfork()) < 0)
  { fprintf(stderr, "vfork error\n");
    return -1;
  } else if (pid == 0)  /* child */
  { g_val++, val++; /* modify parent's variables */
    if (argc == 1) _exit(0);  /* child terminates */
    else exit(0); /* child terminates, with function `close_stdout` atexit */
  }
  /* parent continues here */
  printf("pid = %ld, g_val = %d, val = %d\n",
          (long)getpid(), g_val, val);
  return 0;
}