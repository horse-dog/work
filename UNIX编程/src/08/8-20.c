/**************************************************
 * @file 8-20.c
 * @author your name (you@domain.com)
 * @brief 执行一个解释器文件的程序
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{ pid_t pid;
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
  { if (execlp("./testinterp", "testinterp", "myarg1", "MY ARG2", (char*)0) < 0)
    { fprintf(stderr, "execlp error\n");
      return 1;
    }
  }
  if (waitpid(pid, NULL, 0) < 0)
  { fprintf(stderr, "waitpid error\n");
    return 1;
  }
  return 0;
}