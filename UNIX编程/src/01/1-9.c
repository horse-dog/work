/**************************************************
 * @file 1-9.c
 * @author your name (you@domain.com)
 * @brief 打印用户ID和组ID
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  printf("uid = %d,, gid = %d\n", getuid(), getgid());
  return 0;
}