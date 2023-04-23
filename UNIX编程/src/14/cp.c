#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define COPYINCR (1024*1024*1024) /* 1GB */
int main(int argc, char *argv[])
{ int  fdin, fdout;
  void  *src, *dst;
  size_t    copysz;
  struct stat sbuf;
  off_t    fsz = 0;
  if (argc != 3)
  { printf("usage: %s <fromfile> <tofile>\n", argv[0]);
    return 1;
  }
  fdin  = open(argv[1], O_RDONLY);
  fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
  fstat(fdin, &sbuf);
  ftruncate(fdout, sbuf.st_size);

  while (fsz < sbuf.st_size)
  { if ((sbuf.st_size - fsz) > COPYINCR)
      copysz = COPYINCR;
    else
      copysz = sbuf.st_size - fsz;
    if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED)
    { fprintf(stderr, "mmap error for input\n");
      return 1;
    }
    if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED)
    { fprintf(stderr, "mmap error for output\n");
      return 1;
    }
    memcpy(dst, src, copysz);
    munmap(src, copysz);
    munmap(dst, copysz);
    fsz += copysz;
  }
  return 0;
}