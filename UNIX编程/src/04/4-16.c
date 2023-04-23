/**************************************************
 * @file 4-16.c
 * @author your name (you@domain.com)
 * @brief 打开一个文件，然后unlink它
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ if (open("tempfile", O_RDWR) < 0)
  { fprintf(stderr, "open error\n"); return 1; }
  if (unlink("tempfile") < 0)
  { fprintf(stderr, "unlink error\n"); return 1; }
  printf("file unlinked\n");
  sleep(15);
  printf("done\n");
  return 0;
}