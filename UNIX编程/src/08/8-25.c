/**************************************************
 * @file 8-25.c
 * @author your name (you@domain.com)
 * @brief 打印实际用户ID和有效用户ID
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
int main(void)
{ printf("read uid = %d, effective uid = %d\n",
          getuid(), geteuid());
  return 0;
}