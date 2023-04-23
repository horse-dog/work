/**************************************************
 * @file 4-23.c
 * @author your name (you@domain.com)
 * @brief chdir函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ if (chdir("/tmp") < 0)
  { fprintf(stderr, "chdir failed\n"); return 1; }
  printf("chdir to /tmp succeeded\n");
  return 0;
}