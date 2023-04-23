/**************************************************
 * @file 8-16.c
 * @author your name (you@domain.com)
 * @brief exec 函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

char *env_init[] = { "USR=unknown", "PATH=/tmp", NULL };

int main(void)
{ pid_t pid;
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
  { if (execle("/home/marjason/Desktop/learn/Cpp/Unix/src/08/echoall",
       "echoall", "only 1 arg", (char*)0, env_init) < 0)
    { fprintf(stderr, "execlp error\n");
      return 1;
    }
  }
  if (waitpid(pid, NULL, 0) < 0)
  { fprintf(stderr, "wait error\n");
    return 1;
  }
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
  { if (execlp("./echoall", "echoall", "only 1 arg", (char*)0) < 0)
    { fprintf(stderr, "execlp error\n");
      return 1;
    }
  }
  if (waitpid(pid, NULL, 0) < 0)
  { fprintf(stderr, "wait error\n");
    return 1;
  }
  return 0;
}