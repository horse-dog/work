/**************************************************
 * @file 8-17.c
 * @author your name (you@domain.com)
 * @brief 回显所有命令行参数和所有环境字符串
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ int i;
  char **ptr;
  extern char **environ;
  for (i = 0; i < argc; i++)
    printf("argv[%d]: %s\n", i, argv[i]);
  for (ptr = environ; *ptr != 0; ptr++)
    printf("%s\n", *ptr);
  return 0;
}