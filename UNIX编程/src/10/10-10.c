#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void sig_alrm(int);

int main()
{ int n;
  char line[4096];
  // signal(SIGALRM, sig_alrm); /* we must use SA_INTERUPT flag in our OS */
  
  struct sigaction sa;
  sigset_t set;
  sigemptyset(&set);
  sa.sa_flags = SA_INTERRUPT;
  sa.sa_mask = set;
  sa.sa_handler = sig_alrm;
  sigaction(SIGALRM, &sa, NULL);

  alarm(10);
  /**
   * there's a race condition between `alarm(10)` and `read(...)`.
   * if OS suspend this process between context `alarm(10)` and `read(...)`,
   * process will be resumed after 10s, and then execute `read(...)`,
   * but `read(...)` may block the process permanently.
   */
  if ((n = read(STDIN_FILENO, line, 4096)) < 0)
  { printf("read timeout\n");
    return 1;
  }
  alarm(0); /* if read block time is less than 10s, unset the alarm */
  write(STDOUT_FILENO, line, n);
  return 0;
}

static void sig_alrm(int signo) {}