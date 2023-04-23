#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{ int c;
  while ((c = getchar()) != EOF)
  { if (isupper(c))
      c = tolower(c);
    if (putchar(c) == EOF)
    { fprintf(stderr, "output error\n");
      return 1;
    }
    if (c == '\n')
      fflush(stdout);
  }
  return 0;
}