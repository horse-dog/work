/**************************************************
 * @file 5-4.c
 * @author your name (you@domain.com)
 * @brief 用 getc 和 putc 将标准输入复制到标准输出
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(void)
{ int c;
  while ((c = getc(stdin)) != EOF)
    if (putc(c, stdout) == EOF)
    { fprintf(stderr, "output error\n");
      return 1;
    }
    if (ferror(stdin))
    { fprintf(stderr, "input error\n");
      return 1;
    }
  return 0;
}