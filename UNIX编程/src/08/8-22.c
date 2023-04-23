/**************************************************
 * @file 8-22.c
 * @author your name (you@domain.com)
 * @brief system 函数
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
int system(const char *__cmd) /* version without signal handling */
{ pid_t pid;
  int   status;
  if (__cmd == NULL) return 1;  /* always a command processor with UNIX */
  if ((pid = fork()) < 0) status = -1; /* probably out of processes */
  else if (pid == 0) /* child */
  { execl("/bin/sh", "sh", "-c", __cmd, (char*)0);
    _exit(127); /* execl error */
  } else {  /* parent */
    while (waitpid(pid, &status, 0) < 0)
    { if (errno != EINTR)
      { status = -1;  /* error other than EINTR from waitpid() */
        break;
      }
    }
  }
  return status;
}

int main(int argc, char *argv[])
{ system("echo hello");
  return 0;
}