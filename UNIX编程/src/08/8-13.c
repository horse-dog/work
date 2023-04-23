/**************************************************
 * @file 8-13.c
 * @author your name (you@domain.com)
 * @brief 修改 8-12 程序以避免竞争条件
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr(int signo) { sigflag = 1; }

void TELL_WAIT()
{ signal(SIGUSR1, sig_usr);
  signal(SIGUSR2, sig_usr);
  sigemptyset(&zeromask);
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGUSR1);
  sigaddset(&newmask, SIGUSR2);

  /* block SIGUSR1 and SIGUSR2, and save current signal mask */
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);
}

void TELL_PARENT(pid_t pid)
{ kill(pid, SIGUSR2); }

void WAIT_PARENT()
{ while (sigflag == 0) sigsuspend(&zeromask);
  sigflag = 0;
  
  /* reset signal mask to original value */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void TELL_CHILD(pid_t pid)
{ kill(pid, SIGUSR1); }

void WAIT_CHILD()
{ while (sigflag == 0) sigsuspend(&zeromask);
  sigflag = 0;

  /* reset signal mask to original value */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

static void charatatime(const char*);

int main(void)
{ pid_t pid;
  TELL_WAIT();
  if ((pid = fork()) < 0)
  { fprintf(stderr, "fork error\n");
    return 1;
  }
  else if (pid == 0)
  { charatatime("output from child\n");
    TELL_PARENT(getppid());
  }
  else
  { WAIT_CHILD();
    charatatime("output from parent\n");
  }
  return 0;
}

static void charatatime(const char *str)
{ volatile const char *ptr;
  volatile int c;
  setbuf(stdout, NULL);
  for (ptr = str; (c = *ptr++) != 0; )
    putc(c, stdout);
}