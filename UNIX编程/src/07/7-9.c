/**************************************************
 * @file 7-9.c
 * @author your name (you@domain.com)
 * @brief 进行命令处理程序的典型骨架部分
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <stdio.h>
#include <unistd.h>
#define TOK_ADD 5
#define MAXLINE 4096

void do_line(char *);
void cmd_add(void);
int  get_token(void);

int main(void)
{ char line[MAXLINE];
  while (fgets(line, MAXLINE, stdin) != NULL)
    do_line(line);
  return 0;
}

char *tok_ptr;  /* global pointer for get_token() */

void do_line(char *ptr) /* process one line if input */
{ int cmd;
  tok_ptr = ptr;
  while ((cmd = get_token()) > 0)
  { switch (cmd)
    {
    case TOK_ADD:
      cmd_add();
      break;
    default:
      break;
    }
  }
}

void cmd_add(void)
{ int token;
  token = get_token();
  /* rest of processing for this command */
}

int get_token(void)
{
  /* fetch next token from line pointed to by tok_ptr */
}