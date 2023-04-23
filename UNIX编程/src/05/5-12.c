/**************************************************
 * @file 5-12.c
 * @author your name (you@domain.com)
 * @brief tmpnam 和 tmpfile 函数实例
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(void)
{ char name[L_tmpnam], line[4096];
  FILE *fp;
  printf("%s\n", tmpnam(NULL)); /* first temp name */
  tmpnam(name);
  printf("%s\n", name);
  if ((fp = tmpfile()) == NULL) /* create temp file */
  { fprintf(stderr, "tmpfile error\n");
    return 1;
  }
  fputs("one line of output\n", fp); /* write to temp file */
  rewind(fp); /* then read it back */
  if (fgets(line, sizeof(line), fp) == NULL)
  { fprintf(stderr, "fgets error\n");
    return 1;
  }
  fputs(line, stdout);  /* print the line we wrote */
  return 0;
}