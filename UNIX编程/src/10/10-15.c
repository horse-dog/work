#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
static void sig_quit(int);

int main()
{ sigset_t newmask, oldmask, pendmask;
  signal(SIGQUIT, sig_quit);

  sigemptyset(&newmask);
  sigaddset(&newmask, SIGQUIT);
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  sleep(5); /* SIGQUIT here will remain pending */
  sigpending(&pendmask);
  if (sigismember(&pendmask, SIGQUIT))
    printf("\nSIGQUIT pending\n");
  
  /* restore signal mask which unblocks SIGQUIT */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  /**
   * blocked signal will be delivered to process before `sigprocmask` return.
   * so console will first output: "caught SIGQUIT\n", then output: "SIGQUIT unblocked\n".
   */
  printf("SIGQUIT unblocked\n");
  
  sleep(5); /* SIGQUIT here will terminate with core file */
  return 0;
}

static void sig_quit(int signo)
{ printf("caught SIGQUIT\n");
  signal(SIGQUIT, SIG_DFL);
}