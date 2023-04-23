/**************************************************
 * @file 2-14.c
 * @author your name (you@domain.com)
 * @brief 打印所有可能的 sysconf 和 pathconf 值
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

static void pr_sysconf(char *, int);
static void pr_pathconf(char *, char *, int);

int main(int argc, char *argv[])
{ if (argc != 2)
  { fprintf(stderr, "usage: a.out <dirname>\n");
    return 1;
  }
#ifdef ARG_MAX
  printf("ARG_MAX defined to be %ld\n", (long)ARG_MAX + 0);
#else
  printf("no symbol for ARG_MAX\n");
#endif

#ifdef _SC_ARG_MAX
  pr_sysconf("ARG_MAX = ", _SC_ARG_MAX);
#else
  printf("no symbol for _SC_ARG_MAX\n");
#endif

/* similar processing for all the rest of the sysconf symbols... */
#ifdef MAX_CANON
  printf("MAX_CANON defined to be %ld\n", (long)MAX_CANON + 0);
#else
  printf("no symbol for MAX_CANON\n");
#endif

#ifdef _PC_MAX_CANON
  pr_pathconf("MAX_CANON = ", argv[1], _PC_MAX_CANON);
#else
  printf("no symbol for _PC_MAX_CANON\n");
#endif
/* similar processing for all the rest of the pathconf symbols... */
  return 0;
}

static void pr_sysconf(char *msg, int name)
{ long val;
  fputs(msg, stdout);
  errno = 0;
  if ((val = sysconf(name)) < 0)
  { if (errno != 0)
    { if (errno == EINVAL)
        fputs(" (not supported)\n", stdout);
      else {
        fprintf(stderr, "sysconf error\n");
        _exit(1);
      }
    } else {
      fputs(" (no limit)\n", stdout);
    }
  } else {
    printf(" %ld\n", val);
  }
}

static void pr_pathconf(char *msg, char *path, int name)
{ long val;
  fputs(msg, stdout);
  errno = 0;
  if ((val = pathconf(path, name)) < 0)
  { if (errno != 0)
    { if (errno == EINVAL)
        fputs(" (not supproted)\n", stdout);
      else {
        fprintf(stderr, "sysconf, error\n");
        _exit(1);
      }
    } else {
      fputs(" (no limit)\n", stdout);
    }
  } else {
    printf(" %ld\n", val);
  }
}