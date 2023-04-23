/**************************************************
 * @file 1-8.c
 * @author your name (you@domain.com)
 * @brief 例示 stderror 和 perror
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
  fprintf(stderr, "EACCES: %s\n", strerror(EACCES));
  errno = ENOENT;
  perror(argv[0]);
  return 0;
}