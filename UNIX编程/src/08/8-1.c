/**************************************************
 * @file 8-1.c
 * @author your name (you@domain.com)
 * @brief fork 函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int g_val = 6; /* external variable in initialized data */
char buf[] = "a write to stdout\n";

int main(int argc, const char *argv[])
{ int   val;
  pid_t pid;
  val  = 88;
  if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
  { fprintf(stderr, "write error\n"); return 1; }
  printf("before fork\n");/* we don't flush stdout */
  if ((pid = fork()) < 0)
  { fprintf(stderr, "vfork error\n");
    return 1;
  } else if (pid == 0) {  /* child */
    g_val++, val++; /* modify parent's variables */
  } else {
    sleep(2);
  }
  /* parent continues here */
  printf("pid = %ld, g_val = %d, val = %d\n",
          (long)getpid(), g_val, val);
  return 0;
}