/**************************************************
 * @file 10-5.c
 * @author your name (you@domain.com)
 * @brief 在信号处理程序中调用不可重入函数
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <pwd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static void my_alarm(int signo)
{ struct passwd *rootptr;
  printf("in signal handler\n");
  if ((rootptr = getpwnam("root")) == NULL)
  { fprintf(stderr, "getpwnam(root) error\n"); exit(1); }
  alarm(1);
}

int main()
{ struct passwd *ptr;
  signal(SIGALRM, my_alarm);
  alarm(1);
  for (;;)
  { if ((ptr = getpwnam("marjason")) == NULL)
    { fprintf(stderr, "getpwnam(marjason) error\n"); exit(1); }
    if (strcmp(ptr->pw_name, "marjason") != 0)
      printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
  }
  return 0;
}