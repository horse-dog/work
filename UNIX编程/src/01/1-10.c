/**************************************************
 * @file 1-10.c
 * @author your name (you@domain.com)
 * @brief 从标准输入读命令并执行,支持中断处理
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAXLINE 1024

static void sig_int(int); /* our signal-catching function */

int main(int argc, char *argv[]) {
  char buf[MAXLINE];
  pid_t pid;
  int status;

  if (signal(SIGINT, sig_int) == SIG_ERR) {
    fprintf(stderr, "signal error\n");
    return 1;
  }

  printf("%% "); /* print prompt (printf requires %% to print %) */
  while (fgets(buf, MAXLINE, stdin) != NULL) {
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = 0; /* replace newline with null */
    if ((pid = fork()) < 0) {
      fprintf(stderr, "fork error\n");
      _exit(1);
    } else if (pid == 0) /* child */
    {
      execlp(buf, buf, (char *)0);
      fprintf(stderr, "couldn't execute: %s\n", buf);
      exit(127);
    }
    if ((pid = waitpid(pid, &status, 0)) < 0) {
      fprintf(stderr, "waitpid error\n");
      return 1;
    }
    printf("%% ");
  }
  putchar('\n');
  return 0;
}

void sig_int(int signo) {
  printf("interrupt\n%% ");
  fflush(stdout);
}