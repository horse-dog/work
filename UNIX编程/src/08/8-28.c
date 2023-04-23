/**************************************************
 * @file 8-28.c
 * @author your name (you@domain.com)
 * @brief 产生会计数据的程序
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{ pid_t pid;
  
  pid = fork();
  if (pid != 0)
  { sleep(2);
    exit(2);
  }
  
  pid = fork();
  if (pid != 0)
  { sleep(4);
    abort();
  }
  
  pid = fork();
  if (pid != 0)
  { execl("/bin/dd", "dd", "if=/etc/passwd", "of=/dev/null", NULL);
    exit(7);  /* shouldn't get here */
  }
  
  pid = fork();
  if (pid != 0)
  { sleep(8);
    exit(0);
  }

  sleep(6);
  kill(getpid(), SIGKILL);
  exit(6); /* shouldn't get here */
}