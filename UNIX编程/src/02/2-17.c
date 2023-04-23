/**************************************************
 * @file 2-17.c
 * @author your name (you@domain.com)
 * @brief 确定文件描述符个数
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/resource.h>

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

/**
 * if OPEN_MAX is indeterminate, this might be inadequate
 */
#define OPEN_MAX_GUESS	256

long
open_max(void)
{ if (openmax == 0) /* first time through */
  { errno = 0;
    if ((openmax = sysconf(_SC_OPEN_MAX)) < 0)
    { if (errno == 0)
        openmax = OPEN_MAX_GUESS; /* it's indeterminate */
      else
      { fprintf(stderr, "sysconf error for _SC_OPEN_MAX\n");
        _exit(1);
      }
    }
  }
	return openmax;
}
