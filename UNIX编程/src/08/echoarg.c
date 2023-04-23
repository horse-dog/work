/**************************************************
 * @file echoarg.c
 * @author your name (you@domain.com)
 * @brief 回显所有命令行参数
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ for (int i = 0; i < argc; i++)
    printf("argv[%d]: %s\n", i, argv[i]);
  return 0;
}