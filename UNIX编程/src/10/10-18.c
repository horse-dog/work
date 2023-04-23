#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

__sighandler_t mysignal(int signo, __sighandler_t fn)
{ struct sigaction act, oact;
  act.sa_handler = fn;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (signo == SIGALRM)
    act.sa_flags |= SA_INTERRUPT;
  else
    act.sa_flags |= SA_RESTART;
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return oact.sa_handler;
}

void sig_alarm(int signo)
{ printf("received SIGALRM\n"); }

int main()
{ mysignal(SIGALRM, sig_alarm);
  sigset_t set;
  sigfillset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL); /* block all signal */

  sigemptyset(&set);

  alarm(5);
  /**
   * if OS suspend this process between `alarm(5)` and `sigsuspend(&set)`,
   * and suspend time is longer than 5s, SIGALRM will be blocked for this process,.
   * because we set all signal to block at line 27.
   * then when the process resume, `sigsuspend(&set)` will be deployed,
   * now process get SIGALRM because `set` parameter in `sigsuspend(&set)` is an empty sigset.
   * and then process interupt and goto `sig_alarm` function.
   */
  sleep(10); /* we use `sleep(10)` to simulate a long term suspend */
  sigsuspend(&set);
  printf("resumed...\n");
  return 0;
}