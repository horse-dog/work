/**************************************************
 * @file 7-14.c
 * @author your name (you@domain.com)
 * @brief 自动变量的不正确使用
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>

FILE *open_data(void)
{ FILE *fp;
  char databuf[BUFSIZ]; /* setvbuf makes this the stdio buffer */
  if ((fp = fopen("datafile", "r")) == NULL)
    return NULL;
  if (setvbuf(fp, databuf, _IOLBF, BUFSIZ) != 0)
    return NULL;
  return (fp);
}