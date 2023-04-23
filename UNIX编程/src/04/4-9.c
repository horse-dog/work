/**************************************************
 * @file 4-9.c
 * @author your name (you@domain.com)
 * @brief umask函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main(int argc, char *argv[])
{ umask(0);
  if (creat("foo", RWRWRW) < 0)
  { fprintf(stderr, "creat error for foo\n"); return 1; }
  umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (creat("bar", RWRWRW) < 0)
  { fprintf(stderr, "creat error for bar\n"); return 1; }
  return 0;
}