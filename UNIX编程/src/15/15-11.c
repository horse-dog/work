#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define PAGER "${PAGER:-more}"
int main(int argc, char *argv[])
{ char line[1024];
  FILE *fpin, *fout;
  if (argc != 2)
  { fprintf(stderr, "usage: a.out <pathname>\n"); return 1; }
  fpin = fopen(argv[1], "r");
  fout = popen(PAGER, "w");
  while (fgets(line, 1024, fpin) != NULL)
  { if (fputs(line, fout) == EOF)
      return 1;
  }
  if (ferror(fpin)) return 1;
  pclose(fout);
  return 0;
}