#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define DEF_PAGER "/bin/more"

int main(int argc, char *argv[])
{ int n;
  int fd[2];
  pid_t pid;
  char line[1024];
  FILE *fp;
  
  if (argc != 2)
  { fprintf(stderr, "usage: a.out <pathname>\n"); return 1; }
  fp = fopen(argv[1], "r");
  pipe(fd);
  if ((pid = fork()) < 0)
    return 1;
  else if (pid > 0)
  { close(fd[0]);
    while (fgets(line, 1024, fp) != NULL)
    { n = strlen(line);
      if (write(fd[1], line, n) != n)
      { fprintf(stderr, "write error to pipe\n");
        return 1;
      }
    }
    if (ferror(fp))
    { fprintf(stderr, "fgets error\n");
      return 1;
    }
    close(fd[1]);
    if (waitpid(pid, NULL, 0) < 0)
    { fprintf(stderr, "waitpid error\n");
      return 1;
    }
    return 0;
  } else {
    close(fd[1]);
    if (fd[0] != STDIN_FILENO)
    { dup2(fd[0], STDIN_FILENO);
      close(fd[0]);
    }
    execl(DEF_PAGER, "more", (char*)0);
  }
  return 0;
}