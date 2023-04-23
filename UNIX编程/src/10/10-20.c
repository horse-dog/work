#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

static sigjmp_buf jmpbuf;
static volatile sig_atomic_t canjump;

void
pr_mask(const char *str)
{
	sigset_t	sigset;
	int			errno_save;

	errno_save = errno;		/* we can be called by signal handlers */
	if (sigprocmask(0, NULL, &sigset) < 0) {
		_exit(1);
	} else {
		printf("%s", str);
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");

		/* remaining signals can go here  */

		printf("\n");
	}

	errno = errno_save;		/* restore errno */
}

static void sig_usr1(int signo)
{ time_t starttime;
  if (canjump == 0) return;
  pr_mask("starting sig_sur1: ");
  alarm(3);
  starttime = time(NULL);
  for (;;)
    if (time(NULL) > starttime + 5)
      break;
  pr_mask("finishing sig_usr1: ");
  canjump = 0;
  siglongjmp(jmpbuf, 1);
}

static void sig_alrm(int signo)
{ pr_mask("in sig_alrm: "); }

int main()
{ signal(SIGUSR1, sig_usr1);
  signal(SIGALRM, sig_alrm);
  pr_mask("starting main: ");
  if (sigsetjmp(jmpbuf, 1))
  { pr_mask("ending main: ");
    return 0;
  }
  canjump = 1;
  for (;;) pause();
  return 0;
}