/**************************************************
 * @file 1-4.c
 * @author your name (you@domain.com)
 * @brief 将标准输入复制到标准输出
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#define BUFFSIZE 4096
int main(int argc, char *argv[]) {
  int n;
  char *buf[BUFFSIZE];

  while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
    if (write(STDOUT_FILENO, buf, n) != n) {
      fprintf(stderr, "write error\n");
      return 1;
    }
  }

  if (n < 0) {
    fprintf(stderr, "read error\n");
    return 1;
  }
  return 0;
}