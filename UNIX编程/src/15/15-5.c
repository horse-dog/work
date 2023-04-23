#include <stdio.h>
#include <unistd.h>
int main(void)
{ int     n;
  int fd[2];
  pid_t pid;
  char line[1024];
  pipe(fd);
  if ((pid = fork()) < 0)
    return 1;
  else if (pid > 0)
  { close(fd[0]);
    write(fd[1], "hello world\n", 12);
  } else {
    close(fd[1]);
    n = read(fd[0], line, 1024);
    write(STDOUT_FILENO, line, n);
  }
  return 0;
}