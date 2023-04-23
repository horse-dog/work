/**************************************************
 * @file 8-29.c
 * @author your name (you@domain.com)
 * @brief 打印从系统会计文件中选出的字段
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/acct.h>

#define acct acct_v3
#define FMT "%-*.*s e = %6ld, chars = %7ld, %c %c %c\n"

#if !defined(HAS_ACORE)
#define ACORE 0
#endif
#if !defined(HAS_AXSIG)
#define AXSIG 0
#endif

#if !defined(BSD)
static unsigned long
compt2ulong(comp_t comptime) /* convert comp_t to unsigned long */
{ unsigned long val;
  int           exp;
  val = comptime & 0x1fff; /* 13-bit fraction */
  exp = (comptime >> 13) & 7; /* 3-bit exponent (0-7) */
  while (exp-- > 0)
    val *= 8;
  return val;
}
#endif

int main(int argc, char *argv[])
{ struct acct acdata;
  FILE        *fp;
  if (argc != 2)
  { fprintf(stderr, "usage: pracct filename\n");
    return 1;
  }
  fp = fopen(argv[1], "r");
  if (fp == NULL)
  { fprintf(stderr, "can't open %s\n", argv[1]);
    return 1;
  }
  while (fread(&acdata, sizeof(acdata), 1, fp) == 1)
  { printf(FMT, (int)sizeof(acdata.ac_comm), 
           (int)sizeof(acdata.ac_comm), acdata.ac_comm,
#if defined(BSD)
           acdata.ac_etime, acdata.ac_io,
#else
           compt2ulong(acdata.ac_etime), compt2ulong(acdata.ac_io),
#endif
#if defined(HAS_AC_STAT)
           (unsigned char)acdata.ac_stat,
#endif
           acdata.ac_flag & ACORE ? 'D' : ' ',
           acdata.ac_flag & AXSIG ? 'X' : ' ',
           acdata.ac_flag & AFORK ? 'F' : ' ',
           acdata.ac_flag & ASU   ? 'S' : ' ');
  }
  if (ferror(fp))
  { fprintf(stderr, "read error\n");
    return 1;
  }
  return 0;
}