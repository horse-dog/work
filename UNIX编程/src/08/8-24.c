/**************************************************
 * @file 8-24.c
 * @author your name (you@domain.com)
 * @brief 用 system 执行命令行参数
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

int main(int argc, char *argv[])
{ int status;
  if (argc < 2)
  { fprintf(stderr, "command-line argument required\n");
    return 1;
  }
  if ((status = system(argv[1])) < 0)
  { fprintf(stderr, "system() error\n");
    return 1;
  }
  pr_exit(status);
  return 0;
}