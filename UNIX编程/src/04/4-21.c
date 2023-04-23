/**************************************************
 * @file 4-21.c
 * @author your name (you@domain.com)
 * @brief futimens函数实例
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#ifndef __USE_XOPEN2K8
#define __USE_XOPEN2K8
#endif
#include <sys/stat.h>
int main(int argc, char *argv[])
{ int           i, fd;
  struct stat statbuf;
  struct timespec times[2];
  for (i = 1; i < argc; i++)
  { if (stat(argv[i], &statbuf) < 0) /* fetch current times */
    { fprintf(stderr, "%s: stat error\n", argv[i]);
      continue;
    }
    if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) /* truncate */
    { fprintf(stderr, "%s: open error\n", argv[i]);
      continue;
    }
    times[0] = statbuf.st_atim;
    times[1] = statbuf.st_mtim;
    if (futimens(fd, times) < 0) /* reset times */
      fprintf(stderr, "%s: futimens error\n", argv[i]);
    close(fd);
  }
  return 0;
}