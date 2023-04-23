/**************************************************
 * @file 10-28.c
 * @author your name (you@domain.com)
 * @brief system函数的POSIX.1正确实现
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
static void sig_int(int signo)
{ printf("caught SIGINT\n"); }
static void sig_cld(int signo)
{ printf("caught SIGCHLD\n"); }

int system(const char *__cmd) /* version without signal handling */
{ pid_t pid;
  int   status;
  struct sigaction ignore, saveintr, savequit;
  sigset_t chldmask, savemask;
  
  if (__cmd == NULL) return 1;  /* always a command processor with UNIX */
  
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  ignore.sa_flags = 0;

  if (sigaction(SIGINT, &ignore, &saveintr) < 0)
    return -1;
  if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
    return -1;
  
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
    return -1;
  
  if ((pid = fork()) < 0) status = -1; /* probably out of processes */
  else if (pid == 0) /* child */
  { sigaction(SIGINT, &saveintr, NULL);
    sigaction(SIGQUIT, &savequit, NULL);
    sigprocmask(SIG_SETMASK, &savemask, NULL);

    execl("/bin/sh", "sh", "-c", __cmd, (char*)0);
    _exit(127); /* execl error */
  } else {  /* parent */
    while (waitpid(pid, &status, 0) < 0)
    { if (errno != EINTR)
      { status = -1;  /* error other than EINTR from waitpid() */
        break;
      }
    }
  }

  if (sigaction(SIGINT, &saveintr, NULL) < 0)
    return -1;
  if (sigaction(SIGQUIT, &savequit, NULL) < 0)
    return -1;
  if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
    return -1;

  return status;
}

int main(int argc, char *argv[])
{ signal(SIGINT, sig_int);
  signal(SIGCHLD, sig_cld);
  system("/bin/ed");
  return 0;
}