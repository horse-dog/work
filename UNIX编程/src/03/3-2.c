/**************************************************
 * @file 3-2.c
 * @author your name (you@domain.com)
 * @brief 创建一个具有空洞的文件
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

char buf1[] = "adcdefghij";
char buf2[] = "ABCDEFGHIJ";

int main(int argc, const char *argv[])
{ int fd;
  if ((fd = open("file.hole", O_RDWR | O_CREAT | O_TRUNC, 0b110110100)) < 0)
  { fprintf(stderr, "creat error\n"); return -1; }
  if (write(fd, buf1, 10) != 10)
  { fprintf(stderr, "buf1 write error\n"); return -1; }
  /* offset now = 10 */
  if (lseek(fd, 16384, SEEK_SET) == -1)
  { fprintf(stderr, "lseek error\n"); return -1; }
  /* offset now = 16384 */
  if (write(fd, buf2, 10) != 10)
  { fprintf(stderr, "buf2 write error\n"); return -1; }
  /* offset now = 16394 */
  return 0;
}