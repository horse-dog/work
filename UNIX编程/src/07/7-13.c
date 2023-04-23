/**************************************************
 * @file 7-13.c
 * @author your name (you@domain.com)
 * @brief longjmp 对各类变量的影响
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static void f1(int, int, int, int);
static void f2(void);
static jmp_buf env;
static int globval;

int main(int argc, const char *argv[])
{ int          autoval;
  register int regival;
  volatile int volaval;
  static   int statval;
  globval = 1; autoval = 2; regival = 3; volaval = 4; statval = 5;
  if (setjmp(env) != 0)
  { printf("after longjmp:\n");
    printf("global = %d, autoval = %d, regival = %d, volaval = %d, statval = %d\n",
           globval, autoval, regival, volaval, statval);
    exit(0);
  }
  // change varibales after setjmp, but before longjmp.
  globval = 95; autoval = 96; regival = 97; volaval = 98; statval = 99;
  f1(autoval, regival, volaval, statval);
  exit(0);
}

static void f1(int i, int j, int k, int l)
{ printf("in f1():\n");
  printf("global = %d, autoval = %d, regival = %d, volaval = %d, statval = %d\n",
          globval, i, j, k, l);
  f2();
}
static void f2(void)
{ longjmp(env, 1); }