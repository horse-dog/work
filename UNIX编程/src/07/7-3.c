/**************************************************
 * @file 7-3.c
 * @author your name (you@domain.com)
 * @brief 终止处理程序实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>

static void my_exit1(void);
static void my_exit2(void);

int main(void)
{ if (atexit(my_exit2) != 0)
  { fprintf(stderr, "can't register my_exit2\n");
    return 1;
  }
  if (atexit(my_exit1) != 0)
  { fprintf(stderr, "can't register my_exit1\n");
    return 1;
  }
  if (atexit(my_exit1) != 0)
  { fprintf(stderr, "can't register my_exit1\n");
    return 1;
  }
  printf("main is done\n");
  return 0;
}

static void my_exit1(void)
{ printf("first exit handler\n"); }

static void my_exit2(void)
{ printf("second exit handler\n"); }