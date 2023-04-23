#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static void sig_alrm(int);
static jmp_buf env_alrm;

int main()
{ int n;
  char line[4096];
  signal(SIGALRM, sig_alrm);

  if (setjmp(env_alrm) != 0)
  { printf("read timeout\n");
    return 1;
  }
  alarm(10);
  if ((n = read(STDIN_FILENO, line, 4096)) < 0)
  { printf("read timeout\n");
    return 1;
  }
  alarm(0); /* if read block time is less than 10s, unset the alarm */
  write(STDOUT_FILENO, line, n);
  return 0;
}

static void sig_alrm(int signo)
{ longjmp(env_alrm, 1); }
