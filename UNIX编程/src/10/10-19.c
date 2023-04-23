#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

__sighandler_t signal_intr(int signo, __sighandler_t fn)
{ struct sigaction act, oact;
  act.sa_handler = fn;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_flags |= SA_INTERRUPT;
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return oact.sa_handler;
}

void sig_alrm(int signo)
{ printf("received SIGALRM\n"); }

int main(int argc, char *argv[])
{ int n;
  char buf[1024];
  signal_intr(SIGALRM, sig_alrm);
  alarm(5);
  /* attention, there is a race condition between `alarm(5)` and `if((n = read(...)) < 0)` */
  if ((n = read(STDIN_FILENO, buf, 1024)) < 0)
  { perror(argv[0]);
    return 1;
  }
  alarm(0);
  printf("read: %s", buf);
  return 0;
}