/**************************************************
 * @file 8-23.c
 * @author your name (you@domain.com)
 * @brief 调用 system 函数
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

int main(void)
{ int status;
  status = system("date");
  pr_exit(status);
  status = system("nosuchcommand");
  pr_exit(status);
  status = system("who; exit 44");
  pr_exit(status);
  return 0;
}