#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void sig_usr(int);

int main()
{ signal(SIGUSR1, sig_usr);
  signal(SIGUSR2, sig_usr);
  for (;;) pause();
}

static void sig_usr(int signo)
{ if (signo == SIGUSR1)
    printf("received SIGUSR1\n");
  else if (signo == SIGUSR2)
    printf("received SIGUSR2\n");
  else
    printf("unknown signo\n");
}