#include "log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int level = LOG_INFO;
static int LOG_BUFFER_SIZE = 2048;
static const char* loglevels[] = {"emerge!", "alert!",  "critical!", "error!",
                                  "warn!",   "notice:", "info:",     "debug:"};

void set_loglevel(int log_level) { level = log_level; }

void log(int log_level, const char* file_name, int line_num, const char* format,
         ...) {

  // 首先排除被日志掩码屏蔽的日志信息
  if (log_level > level) {
    return;
  }
  // 获取当前系统时间
  time_t tmp = time(NULL);

  // 转换为年月日时分秒类型的时间信息
  struct tm* cur_time = localtime(&tmp);
  if (!cur_time) {
    return;
  }

  // 格式化输出日志信息到标准输出
  char arg_buffer[LOG_BUFFER_SIZE];
  memset(arg_buffer, '\0', LOG_BUFFER_SIZE);
  strftime(arg_buffer, LOG_BUFFER_SIZE - 1, "[ %x %X ] ", cur_time);
  printf("%s", arg_buffer);
  printf("%s:%04d ", file_name, line_num);
  printf("%s ", loglevels[log_level - LOG_EMERG]);

  va_list arg_list;
  va_start(arg_list, format);
  memset(arg_buffer, '\0', LOG_BUFFER_SIZE);
  vsnprintf(arg_buffer, LOG_BUFFER_SIZE - 1, format, arg_list);
  printf("%s\n", arg_buffer);
  fflush(stdout);
  va_end(arg_list);
}
