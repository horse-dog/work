#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t quitflag; /* set nonzero by signal handler */

static void sig_int(int signo)
{ if (signo == SIGINT) printf("\ninterrupt\n");
  else if (signo == SIGQUIT)
    quitflag = 1; /* set flag for main loop */
}

int main()
{ sigset_t newmask, oldmask, zeromask;
  signal(SIGINT, sig_int);
  signal(SIGQUIT, sig_int);

  sigemptyset(&zeromask);
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGQUIT);

  /* block SIGQUIT and save current signal mask */
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  while (quitflag == 0)
    sigsuspend(&zeromask);
  /* SIGQUIT has been caught and is now blocked; do whatever. */
  quitflag = 0;
  /* reset signal mask which unblocks SIGQUIT */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  return 0;
}