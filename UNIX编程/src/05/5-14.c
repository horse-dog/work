/**************************************************
 * @file 5-14.c
 * @author your name (you@domain.com)
 * @brief 观察内存流的写入操作
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BSZ 48

int main()
{ FILE *fp;
  char buf[BSZ];
  memset(buf, 'a', BSZ - 2);
  buf[BSZ - 2] = '\0';
  buf[BSZ - 1] = 'X';
  if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
  { fprintf(stderr, "fmemopen failed\n");
    return 1;
  }
  printf("initial buffer contents: %s\n", buf);
  fprintf(fp, "hello, world");
  printf("before flush: %s\n", buf);
  fflush(fp);
  printf("after fflush: %s\n", buf);
  printf("len of string in buf = %ld\n", (long)strlen(buf));

  memset(buf, 'b', BSZ - 2);
  buf[BSZ - 2] = '\0';
  buf[BSZ - 1] = 'X';
  fprintf(fp, "hello, world");
  fseek(fp, 0, SEEK_SET);
  printf("after fseek: %s\n", buf);
  printf("len of string in buf = %ld\n", (long)strlen(buf));

  memset(buf, 'c', BSZ - 2);
  buf[BSZ - 2] = '\0';
  buf[BSZ - 1] = 'X';
  fprintf(fp, "hello, world");
  fclose(fp);
  printf("after fclose: %s\n", buf);
  printf("len of string in buf = %ld\n", (long)strlen(buf));
  return 0;
}