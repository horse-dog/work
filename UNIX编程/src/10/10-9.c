#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static jmp_buf env_alrm;

unsigned int mysleep(unsigned int);
static void sig_int(int);

int main()
{ unsigned int unslept;
  signal(SIGINT, sig_int);
  unslept = mysleep(5);
  printf("sleep returned: %u\n", unslept);
  return 0;
}

static void sig_int(int signo)
{ int i, j;
  volatile int k;
  printf("\nsig_int starting...\n");
  for (i = 0; i < 300000; i++)
    for (j = 0; j < 300000; j++)
      k += i * j;
  printf("sig_int finished...\n");
}

static void sig_alrm(int signo)
{ longjmp(env_alrm, 1); }

unsigned int mysleep(unsigned int seconds)
{ if (signal(SIGALRM, sig_alrm) == SIG_ERR)
    return seconds;
  if (setjmp(env_alrm) == 0)
  { alarm(seconds);
    pause();
  }
  return alarm(0);  /* turn off timer, return unslept time */
}