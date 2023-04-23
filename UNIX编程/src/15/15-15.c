#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{ char  line[1024];
  FILE *fpin = popen("./myuclc", "r");
  for ( ; ; )
  { fputs("prompt> ", stdout);
    fflush(stdout);
    if (fgets(line, 1024, fpin) == NULL) /* read from pipe */
      break;
    fputs(line, stdout);
  }
  pclose(fpin);
  putchar('\n');
  return 0;
}