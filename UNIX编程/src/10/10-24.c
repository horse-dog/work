#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

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