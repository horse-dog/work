/**************************************************
 * @file 5-11.c
 * @author your name (you@domain.com)
 * @brief 对各个标准I/O流打印缓冲状态信息
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>

void pr_stdio(const char *, FILE *);
int is_unbuffered(FILE *);
int is_linebuffered(FILE *);
int buffer_size(FILE *);

int main(void)
{ FILE *fp;
  fputs("enter any character\n", stdout);
  if (getchar() == EOF)
  { fprintf(stderr, "getchar error\n");
    return 1;
  }
  fputs("one line to stderr\n", stderr);

  pr_stdio("stdin", stdin);
  pr_stdio("stdout", stdout);
  pr_stdio("stderr", stderr);

  if ((fp = fopen("/etc/passwd", "r")) == NULL)
  { fprintf(stderr, "fopen error\n");
    return 1;
  }
  if (getc(fp) == EOF)
  { fprintf(stderr, "getc error\n");
    return 1;
  }
  pr_stdio("/etc/passwd", fp);
  return 0;
}

void pr_stdio(const char *name, FILE *fp)
{ printf("stream = %s, ", name);
  if (is_unbuffered(fp))
    printf("unbuffered");
  else if (is_linebuffered(fp))
    printf("line buffered");
  else
    printf("fully buffered");
  printf(", buffer size = %d\n", buffer_size(fp));
}

int is_unbuffered(FILE *fp)
{ return fp->_flags & 0X1; }

int is_linebuffered(FILE *fp)
{ return fp->_flags & 0x200; }

int buffer_size(FILE *fp)
{ return fp->_IO_buf_end - fp->_IO_buf_base; }