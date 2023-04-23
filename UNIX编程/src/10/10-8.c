#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static jmp_buf env_alrm;

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

int main(int argc, char *argv[])
{ mysleep(5);
  return 0;
}