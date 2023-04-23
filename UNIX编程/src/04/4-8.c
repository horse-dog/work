/**************************************************
 * @file 4-8.c
 * @author your name (you@domain.com)
 * @brief access函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{ if (argc != 2)
  { fprintf(stderr, "usage: a.out <pathname>\n"); return 1; }
  if (access(argv[1], R_OK) < 0)
    fprintf(stderr, "access error for %s\n", argv[1]);
  else 
    printf("read access OK\n");
  if (open(argv[1], O_RDONLY) < 0)
    fprintf(stderr, "open error for %s\n", argv[1]);
  else
    printf("open for reading OK\n");
  return 0;
}