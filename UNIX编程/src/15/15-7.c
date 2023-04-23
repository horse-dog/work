#include <stdio.h>
#include <unistd.h>

static int pfd1[2], pfd2[2];

void TELL_WAIT(void)
{ pipe(pfd1);
  pipe(pfd2);
}

void TELL_PARENT(pid_t pid)
{ write(pfd2[1], "c", 1);
  return;
}

void WAIT_PARENT(void)
{ char c;
  read(pfd1[0], &c, 1);
}

void TELL_CHILD(pid_t pid)
{ write(pfd1[1], "p", 1);
  return;
}

void WAIT_CHILD(void)
{ char c;
  read(pfd2[0], &c, 1);
}