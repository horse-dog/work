/**************************************************
 * @file 7-4.c
 * @author your name (you@domain.com)
 * @brief 将所有命令行参数回显到标准输出
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
int main(int argc, char *argv[])
{ int i;
  for (i = 0; i < argc; i++)
    printf("argv[%d]: %s\n", i, argv[i]);
  return 0;
}