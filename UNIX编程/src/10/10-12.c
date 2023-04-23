#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define SIGBAD(signo) ((signo) <= 0 || (signo) >= 32)

void printset(sigset_t *set)
{ unsigned int *p = (unsigned int *)set;
  unsigned int value = p[0];
  for (int i = 31; i >= 0; i--)
    putchar(((value >> i) & 1) ? '1' : '0');
  putchar('\n');
}

int my_sigaddset(sigset_t *set, int signo)
{ if (SIGBAD(signo))
  { errno = EINVAL;
    return -1;
  }
  unsigned int *p = (unsigned int *)set;
  *p |= 1 << (signo - 1);
  return 0;
}

int my_sigdelset(sigset_t *set, int signo)
{ if (SIGBAD(signo))
  { errno = EINVAL;
    return -1;
  }
  unsigned int *p = (unsigned int *)set;
  *p &= ~(1 << (signo - 1));
  return 0;
}

int my_sigismember(const sigset_t *set, int signo)
{ if (SIGBAD(signo))
  { errno = EINVAL;
    return -1;
  }
  unsigned int *p = (unsigned int *)set;
  return ((*p & (1 << (signo - 1))) != 0);
}

int main()
{ sigset_t set;
  sigemptyset(&set);
  printset(&set);
  my_sigaddset(&set, SIGHUP);
  printset(&set);
  my_sigdelset(&set, SIGHUP);
  printset(&set);
  return 0;
}