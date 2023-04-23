/**************************************************
 * @file 10-29.c
 * @author your name (you@domain.com)
 * @brief system函数的POSIX.1正确实现
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
static void sig_alrm(int signo)
{ /* nothing to do, just returning wakes up sigsuspend() */ }

unsigned int mysleep(unsigned int seconds)
{ struct sigaction newact, oldact;
  sigset_t newmask, oldmask, suspmask;
  unsigned int unslept;
  /* set our handler, save previous information */
  newact.sa_handler = sig_alrm;
  sigemptyset(&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction(SIGALRM, &newact, &oldact);
  /* block SIGALRM and save current signal mask */
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGALRM);
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);

  alarm(seconds);
  suspmask = oldmask;

  /* make sure SIGALRM isn't blocked */
  sigdelset(&suspmask, SIGALRM);

  /* wait for any signal to be caught */
  sigsuspend(&suspmask);

  /* some signal has been caught, SIGALRM is now blocked */
  unslept = alarm(0);

  /* reset previous action */
  sigaction(SIGALRM, &oldact, NULL);
  
  /* reset signal mask, which unblocks SIGALRM */
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  return unslept;
}

int main(int argc, char *argv[])
{ mysleep(5);
  return 0;
}