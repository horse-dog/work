#ifndef LOG_H
#define LOG_H

#include <syslog.h>

#include <cstdarg>

// 设置日志掩码
void set_loglevel(int log_level = LOG_DEBUG);

/**
 * @brief 打印日志
 * 
 * @param log_level 日志等级 
 * @param file_name 产生日志的代码文件
 * @param line_num 产生日志的代码行数
 * @param format 格式化串
 * @param ... 不定参数
 */
void log(int log_level, const char* file_name, int line_num, const char* format,
         ...);

#endif
