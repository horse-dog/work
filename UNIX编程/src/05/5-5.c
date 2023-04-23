/**************************************************
 * @file 5-5.c
 * @author your name (you@domain.com)
 * @brief 用 fgets 和 fputs 将标准输入复制到标准输出
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(void)
{ char buf[1024];
  while (fgets(buf, 1024, stdin) != NULL)
    if (fputs(buf, stdout) == EOF)
    { fprintf(stderr, "output error\n");
      return 1;
    }
  if (ferror(stdin))
  { fprintf(stderr, "input error\n");
    return 1;
  }
  return 0;
}