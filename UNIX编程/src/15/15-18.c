#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static void sig_pipe(int signo)
{ printf("SIGPIPE caught\n");
  exit(1);
}

int main(void)
{ int   n, fd1[2], fd2[2];
  pid_t pid;
  char  line[1024];

  if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
    return 1;
  pipe(fd1) & pipe(fd2);
  if ((pid = fork()) < 0)
    return 1;
  else if (pid > 0)
  { close(fd1[0]) & close(fd2[1]);
    while (fgets(line, 1024, stdin) != NULL)
    { n = strlen(line);
      write(fd1[1], line, n);
      n = read(fd2[0], line, 1024);
      if (n == 0)
      { puts("child closed pipe");
        break;
      }
      line[n] = 0;
      fputs(line, stdout);
    }
    if (ferror(stdin)) return 1;
    return 0;
  } else {
    close(fd1[1]) & close(fd2[0]);
    if (fd1[0] != STDIN_FILENO)
    { dup2(fd1[0], STDIN_FILENO);
      close(fd1[0]);
    }
    if (fd2[1] != STDOUT_FILENO)
    { dup2(fd2[1], STDOUT_FILENO);
      close(fd2[1]);
    }
    execl("./add2", "add2", (char*)0);
    _exit(127);
  }
  return 0;
}