#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{ int n, int1, int2;
  char line[1024];
  while ((n = read(STDIN_FILENO, line, 1024)) > 0)
  { line[n] = 0;
    if (sscanf(line, "%d%d", &int1,  &int2) == 2)
    { sprintf(line, "%d\n", int1 + int2);
      n = strlen(line);
      write(STDOUT_FILENO, line, n);
    } else {
      write(STDOUT_FILENO, "invalid args\n", 13);
    }
  }
  return 0;
}