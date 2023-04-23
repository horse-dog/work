/**************************************************
 * @file 1-6.c
 * @author your name (you@domain.com)
 * @brief 打印进程ID
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  printf("hello world from process ID %ld\n", (long)getpid());
  return 0;
}