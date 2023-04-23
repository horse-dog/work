/**************************************************
 * @file 10-6.c
 * @author your name (you@domain.com)
 * @brief 不能正常工作的 System V SIGCLD 处理程序
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static void sig_cld(int);

int main(int argc, char *argv[])
{ pid_t pid;
  /* macro SIGCLD equals with SIGCHLD in Linux */
  if (signal(SIGCLD, sig_cld) == SIG_ERR)
  { fprintf(stderr, "signal error\n"); return 1; }
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n"); return 1; }
  else if (pid == 0)  /* child */
  { sleep(2);
    _exit(0);
  }
  pause();  /* parent */
  return 0;
}

static void sig_cld(int signo)  /* interrupts pause() */
{ pid_t pid;
  int   status;
  printf("SIGCLD received\n");
  if (signal(SIGCLD, sig_cld) == SIG_ERR) /* reestablish handler */
  { fprintf(stderr, "signal error\n"); }
  if ((pid = wait(&status)) < 0)  /* fetch child status */
  { fprintf(stderr, "wait error\n"); }
  printf("pid = %d\n", pid);
}