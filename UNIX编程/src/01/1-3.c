/**************************************************
 * @file 1-3.c
 * @author your name (you@domain.com)
 * @brief 列出一个目录中的所有文件
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {

  DIR *dp;
  struct dirent *dirp;
  
  if (argc != 2) {
    fprintf(stderr, "usage: a.out directory_name\n");
    return 1;
  }
  
  if ((dp = opendir(argv[1])) == NULL) {
    fprintf(stderr, "can't open %s\n", argv[1]);
    return 1;
  }
  
  while ((dirp = readdir(dp)) != NULL) 
    printf("%s\n", dirp->d_name);

  closedir(dp);
  return 0;

}