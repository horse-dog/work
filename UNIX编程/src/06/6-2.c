/**************************************************
 * @file 6-2.c
 * @author your name (you@domain.com)
 * @brief getpwnam 函数
 * @version 0.1
 * @date 2022-10-20
 * @copyright Copyright (c) 2022
 **************************************************/
#include <pwd.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

struct passwd *
mygetpwnam(const char *name)
{ struct passwd *ptr;
  setpwent();
  while ((ptr = getpwent()) != NULL)
    if (strcmp(name, ptr->pw_name) == 0)
      break;
  endpwent();
  return ptr;
}

int main()
{ struct passwd *pwd = mygetpwnam("marjason");
  printf("name   :%s\n", pwd->pw_name);
  printf("passwd :%s\n", pwd->pw_passwd);
  printf("~dir   :%s\n", pwd->pw_dir);
  printf("uid    :%d\n", pwd->pw_uid);
  printf("gid    :%d\n", pwd->pw_gid);
  printf("shell  :%s\n", pwd->pw_shell);
  return 0;
}