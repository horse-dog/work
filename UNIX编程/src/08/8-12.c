/**************************************************
 * @file 8-12.c
 * @author your name (you@domain.com)
 * @brief 带有竞争条件的程序
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static void charatatime(const char*);

int main(void)
{ pid_t pid;
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
    charatatime("output from child\n");
  else
    charatatime("output from parent\n");
  return 0;
}

static void charatatime(const char *str)
{ volatile const char *ptr;
  volatile int c;
  setbuf(stdout, NULL);
  for (ptr = str; (c = *ptr++) != 0; )
    putc(c, stdout);
}