#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

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

static void sig_int(int signo)
{ pr_mask("\nin sig_int: "); }

int main()
{ sigset_t newmask, oldmask, waitmask;
  pr_mask("program start: ");
  signal(SIGINT, sig_int);

  sigemptyset(&waitmask);
  sigaddset(&waitmask, SIGUSR1);
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGINT);

  /* block SIGINT and save current signal mask */
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  /* critical region of code */
  pr_mask("in critical region: ");

  /* pause, allowing all signals except SIGUSR1 */
  sigsuspend(&waitmask);
  pr_mask("after return from sigsuspend: ");

  /* reset signal mask which unblocks SIGINT */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);

  /* and continue processing... */
  pr_mask("program exit: ");
  return 0;
}