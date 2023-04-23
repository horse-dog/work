/**************************************************
 * @file 10-7.c
 * @author your name (you@domain.com)
 * @brief sleep简化而不完整的实现
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
static void sig_alrm(int signo)
{ /* nothing to do, just returning wakes up sigsuspend() */ }

unsigned int mysleep(unsigned int seconds)
{ if (signal(SIGALRM, sig_alrm) == SIG_ERR)
    return seconds;
  alarm(seconds); /* start the timer */
  pause();  /* next caught signal wakes us up */
  return alarm(0);  /* turn off timer, return unslept time */
}

int main(int argc, char *argv[])
{ mysleep(5);
  return 0;
}