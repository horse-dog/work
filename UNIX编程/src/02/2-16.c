/**************************************************
 * @file 2-16.c
 * @author your name (you@domain.com)
 * @brief 为路径名动态地分配空间
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

/* If PATH_MAX is in determinate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

char *path_alloc(size_t *sizep) /* also return allocated, if nonnull */
{ char *ptr;
  size_t size;
  if (posix_version == 0)
    posix_version = sysconf(_SC_VERSION);
  if (xsi_version == 0)
    xsi_version = sysconf(_SC_XOPEN_VERSION);

  if (pathmax == 0) /* first time through */
  { errno = 0;
    if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0)
    { if (errno == 0)
        pathmax = PATH_MAX_GUESS; /* it's indeterminate */
      else
      { fprintf(stderr, "pathconf error for _PC_PATH_MAX)\n");
        return 1;
      }
    } else {
      pathmax++;  /* add one since it's relative to root */
    }
  }
  /**
   * Before POSIX.1-2001, we aren't guaranteed that PATH_MAX includes
   * the terminating null byte. Same goes for XPG3.
   */
  if ((posix_version < 200112L) && (xsi_version < 4))
    size = pathmax + 1;
  else
    size = pathmax;
  if ((ptr = malloc(size)) == NULL)
  { fprintf(stderr, "malloc error for pathname\n");
    return 1;
  }
  if (sizep != NULL)
    *sizep = size;
  return ptr;
}