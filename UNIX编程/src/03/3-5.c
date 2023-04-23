/**************************************************
 * @file 3-5.c
 * @author your name (you@domain.com)
 * @brief 只使用 read 和 write 函数复制一个文件
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <unistd.h>
#include <stdio.h>
#define BUFFSIZE 4096
int main(int argc, const char *argv[])
{ int n;
  char buf[BUFFSIZE];
  while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    if (write(STDOUT_FILENO, buf, n) != n)
    { fprintf(stderr, "write error\n"); return -1; }
  if (n < 0) { fprintf(stderr, "read error\n"); }
  return 0;
}