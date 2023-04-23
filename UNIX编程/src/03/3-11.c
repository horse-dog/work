/**************************************************
 * @file 3-11.c
 * @author your name (you@domain.com)
 * @brief 第一个参数指定文件描述符，并对该描述符打印其所选择的文件标志说明
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{ int val;
  if (argc != 2)
  { fprintf(stderr, "usage: a.out <descriptor#>\n");
    return -1;}
  if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
  { fprintf(stderr, "fcntl error for fd %d\n", atoi(argv[1]));
    return -1; }
  switch (val & O_ACCMODE) 
  {
  case O_RDONLY:
    printf("read only"); break;
  case O_WRONLY:
    printf("write only"); break;
  case O_RDWR:
    printf("read write"); break;
  default:
    fprintf(stderr, "unknown access mode");
  }
  if (val & O_APPEND) printf(", append");
  if (val & O_NONBLOCK) printf(", nonblocking");
  if (val & O_SYNC) printf(", synchronous writes");
  if (val & O_FSYNC) printf(", synchronous writes");
  putchar('\n');
  return 0;
}