/**************************************************
 * @file 4-25.c
 * @author your name (you@domain.com)
 * @brief 打印st_dev和st_rdev值
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
int main(int argc, char *argv[])
{ int           i;
  struct stat buf;
  for (i = 1; i < argc; i++)
  { printf("%s: ", argv[i]);
    if(stat(argv[i], &buf) < 0)
    { fprintf(stderr, "stat error\n");
      continue;
    }
    printf("dev = %d/%d", major(buf.st_dev), minor(buf.st_dev));
    if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))
    { printf(" (%s) rdev = %d/%d",
        (S_ISCHR(buf.st_mode)) ? "character" : "block",
        major(buf.st_rdev), minor(buf.st_rdev));
    }
    printf("\n");
  }
  return 0;
}