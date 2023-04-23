/**************************************************
 * @file 8-8.c
 * @author your name (you@domain.com)
 * @brief fork 两次以避免僵死进程
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{ pid_t pid;
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
  { if ((pid = fork()) < 0)
    { fprintf(stderr, "fork error\n");
      return 1;
    }
    else if (pid > 0) {
      printf
      (
        "first child, pid = %ld, ppid = %ld, pgid = %ld, sid = %ld\n",
        (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(getpid())
      );
      exit(0);
    }

    setsid();
    printf
    (
      "second child, pid = %ld, ppid = %ld, pgid = %ld, sid = %ld\n",
      (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(getpid())
    );
    sleep(10); /* simulate long time process */
    exit(0);
  }

  if (waitpid(pid, NULL, 0) != pid)
  { fprintf(stderr, "waitpid error\n");
    return 1;
  }
  /* do our own work... */
  printf
  (
    "parent, pid = %ld, ppid = %ld, pgid = %ld, sid = %ld\n",
    (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(getpid())
  );
  return 0;
}
