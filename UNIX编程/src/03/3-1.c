/**************************************************
 * @file 3-1.c
 * @author your name (you@domain.com)
 * @brief 测试标准输入能否被设置偏移量
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, const char *argv[])
{ if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
    printf("cannot seek\n");
  else
    printf("seek OK\n");
  return 0;
}