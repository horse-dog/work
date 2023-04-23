/**************************************************
 * @file 3-12.c
 * @author your name (you@domain.com)
 * @brief 对一个文件描述符开启一个或多个文件状态标志
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <stdio.h>
void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{ int val;
  if ((val = fcntl(fd, F_GETFL, 0)) < 0)
  { fprintf(stderr, "fcntl F_GETFL error\n"); return; }
  val |= flags; /* turn on flags */
  if (fcntl(fd, F_SETFL, val) < 0)
  { fprintf(stderr, "fcntl F_SETFL error\n"); return; }
}
