#include <stdio.h>
int main(void)
{ int int1, int2;
  char line[1024];

  setvbuf(stdin, NULL, _IOLBF, 0);
  setvbuf(stdout, NULL, _IOLBF, 0);

  while (fgets(line, 1024, stdin) != NULL)
  { if (sscanf(line, "%d%d", &int1, &int2) == 2)
      printf("%d\n", int1 + int2);
    else
      printf("invalid args\n");
  }
}