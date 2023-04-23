/**************************************************
 * @file 8-31.c
 * @author your name (you@domain.com)
 * @brief 计时并执行所有命令行参数
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/times.h>

static void pr_exit(int);
static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

int main(int argc, char *argv[])
{ int i;
  setbuf(stdout, NULL);
  for (i = 1; i < argc; i++)
    do_cmd(argv[i]);
  return 0;
}

static void do_cmd(char *cmd)
{ struct tms tmsstart, tmsend;
  clock_t start, end;
  int status;
  printf("\ncommand: %s\n", cmd);
  start = times(&tmsstart);
  status = system(cmd);
  if (status < 0)
  { perror("system() error");
    exit(1);
  }
  end = times(&tmsend);
  pr_times(end - start, &tmsstart, &tmsend);
  pr_exit(status);
}

static void
pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{ static long clktck = 0;
  if (clktck == 0)
    clktck = sysconf(_SC_CLK_TCK);
  printf(" real: %7.2f\n", real / (double)clktck);
  printf(" user: %7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
  printf(" child user: %7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime) / (double)clktck);
  printf(" child sys:  %7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime) / (double)clktck);
}

void pr_exit(int status)
{ if (WIFEXITED(status))
    printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
  else if (WIFSIGNALED(status))
    printf("abnormal termination, signal number = %d%s\n", 
            WTERMSIG(status), 
#ifdef WCOREDUMP
            WCOREDUMP(status) ? " (core file generated)" : "");
#else
            "");
#endif
  else if (WIFSTOPPED(status))
    printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}