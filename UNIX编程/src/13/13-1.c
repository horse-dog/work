/**************************************************
 * @file 13-1.c
 * @author your name (you@domain.com)
 * @brief 创建守护进程
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/

#include <syslog.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void daemonize(const char* cmd) {

  int i, fd0, fd1, fd2;
  pid_t pid;
  struct rlimit rl;
  struct sigaction sa;

  // clear file creation mask
  umask(0);

  // get maximum number of file descriptors
  if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
    printf("can't get file limit");
    exit(1);
  }

  // become a session leader to lose controlling TTY
  if ((pid = fork()) < 0) {
    printf("fork error");
    exit(1);
  }

  if (pid != 0) exit(0);

  setsid();

  // ensure future opens won't allocate controlling TTYS.
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGHUP, &sa, NULL) < 0) {
    printf("can't ignore SIGHUP");
    exit(1);
  }

  // another fork, ensure the process is not session leader
  // so that we can prevent it get a TTY anytime.
  if ((pid = fork()) < 0) {
    printf("fork error");
    exit(1);
  }

  if (pid != 0) exit(0);

  // change the current working directory to the root so
  // we won't prevent file systems from being unmounted.

  if (chdir("/") < 0) {
    printf("chdir error");
    exit(1);
  }

  // close all open file descriptors
  if (rl.rlim_max == RLIM_INFINITY)
    rl.rlim_max = 1024;

  for (i = 0; i < rl.rlim_max; i++)
    close(i);

  // attach file descriptors 0, 1, 2 to /dev/null
  fd0 = open("dev/null", O_RDWR);
  fd1 = dup(0);
  fd2 = dup(0);

  // initialze the log file
  openlog(cmd, LOG_CONS, LOG_DAEMON);

  if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
    exit(1);
  }

}