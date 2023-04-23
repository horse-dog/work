/**************************************************
 * @file 4-24.c
 * @author your name (you@domain.com)
 * @brief getcwd函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ char    *ptr;
  size_t  size;
  if (chdir("/usr/doc") < 0)
  { fprintf(stderr, "chdir failed\n"); return 1; }
  size = 4096;
  ptr = (char *)malloc(size);
  if (getcwd(ptr, size) == NULL)
    fprintf(stderr, "getcwd failed\n");
  else
    printf("cwd = %s\n", ptr);
  free(ptr);
  return 0;
}