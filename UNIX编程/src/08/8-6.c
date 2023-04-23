/**************************************************
 * @file 8-6.c
 * @author your name (you@domain.com)
 * @brief 演示不同的 exit 值
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void pr_exit(int status)
{ if (WIFEXITED(status))
    printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
  else if (WIFSIGNALED(status))
    printf("abnormal termination, signal number = %d%s\n", 
            WTERMSIG(status), 
#ifdef WCOREDUMP
            WCOREDUMP(status) ? " (core file generated)" : "");
#else
            "");
#endif
  else if (WIFSTOPPED(status))
    printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

int main()
{ pid_t pid;
  int   status;
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
    exit(7);

  if (wait(&status) != pid)
  { fprintf(stderr, "wait error\n");
    return 1;
  }

  pr_exit(status);

  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
    abort();
  
  if (wait(&status) != pid)
  { fprintf(stderr, "wait error\n");
    return 1;
  }

  pr_exit(status);

  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
    status /= 0;
  
  if (wait(&status) != pid)
  { fprintf(stderr, "wait error\n");
    return 1;
  }

  pr_exit(status);
  return 0;
}