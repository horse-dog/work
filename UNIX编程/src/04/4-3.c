/**************************************************
 * @file 4-3.c
 * @author your name (you@domain.com)
 * @brief 对每个命令行参数打印文件类型
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
int main(int argc, char *argv[])
{ int           i;
  struct stat buf;
  char       *ptr;
  for (i = 1; i < argc; i++)
  { printf("%s: ", argv[i]);
    if (lstat(argv[i], &buf) < 0)
    { fprintf(stderr, "lstat error\n");
      continue; 
    }
    if (S_ISREG(buf.st_mode)) ptr = "regular";
    else if (S_ISDIR(buf.st_mode)) ptr = "directory";
    else if (S_ISCHR(buf.st_mode)) ptr = "character special";
    else if (S_ISBLK(buf.st_mode)) ptr = "block special";
    else if (S_ISFIFO(buf.st_mode))ptr = "fifo";
    else if (S_ISLNK(buf.st_mode)) ptr = "symbolic link";
    else ptr = "** unknown mode **";
    printf("%s\n", ptr);
  }
  return 0;
}