/**************************************************
 * @file 8-5.c
 * @author your name (you@domain.com)
 * @brief 打印 exit 状态的说明
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
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
{ pr_exit(0);
  pr_exit(1);
  pr_exit(-1);
  return 0;
}