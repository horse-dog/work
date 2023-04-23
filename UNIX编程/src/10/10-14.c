#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

void pr_mask(const char *str)
{ sigset_t sigset;
  int  errno_save;
  errno_save = errno;
  sigprocmask(0, NULL, &sigset);
  printf("%s", str);
  if (sigismember(&sigset, SIGINT))
    printf(" SIGINT");
  if (sigismember(&sigset, SIGQUIT))
    printf(" SIGQUIT");
  if (sigismember(&sigset, SIGUSR1))
    printf(" SIGUSR1");
  if (sigismember(&sigset, SIGUSR2))
    printf(" SIGUSR2");
  if (sigismember(&sigset, SIGALRM))
    printf(" SIGALRM");
  printf("\n");
  errno = errno_save;
}

int main()
{ pr_mask("current mask:");
  return 0;
}