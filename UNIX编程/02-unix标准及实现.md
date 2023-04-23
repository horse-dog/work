## 1 UNIX标准化
### 1.1 ISO C
由于C语言在UNIX系统中的地位不言而喻，C语言的标准化也是UNIX标准化的重要组成部分。ISO C标准当中包括了
- 语法和语义
- 标准库

时间线：1989（发布）-> 1999（更新）-> 2001 2004 2007 （勘误）-> 2011（更新）

### 1.2 IEEE POSIX
- POSIX(Portable Operating System Interface，可移植操作系统接口)
- POSIX是由IEEE制订的一系列标准，其指的是可移植操作系统接口，它说明的是接口而不是实现。

### 1.3 Single UNIX Specification
Single UNIX Specification简称SUS，它是POSIX.1标准的一个超集。

## 2 UNIX系统实现
| 系统 | 实现 |
| ----------- | ----------- |
| SVR4 | AT&T实现 |
| BSD | 加州大学伯克利分校实现 |
| FreeBSD | FreeBSD志愿者 |
| Linux | Minix改写而来，志愿者维护 |
| Mac OS X | Apple公司 |
| Solaris | SUN公司 |

## 3 限制
- 编译时限制: 通常与编译器有关
- 运行时限制: 通常与操作系统有关
- sysconf函数
```cpp
/**
 * @brief 获取系统执行的配置信息
 * @param __name 限制名
 * @return 限制值
 * @exception -1 限制值是不确定的
 * @exception -1, 置errno为EINVAL __name参数不是一个合适的常量
 */
extern long int sysconf (int __name) __THROW;
```
| 限制名 | 说明 | name参数 |
| ----------- | ----------- | ----------- |
| ARG_MAX | exec函数的参数最大长度(字节) | `_SG_ARG_MAX` |
| ATEXIT_MAX | 可用`atexit`函数登记的最大函数个数 | `_SG_ATEXIT_MAX` |
| CHILD_MAX | 每个实际用户ID的最大进程数 | `_SC_CHILD_MAX` |
| 时钟滴答/秒 | 每秒时钟滴答数 | `_SC_CLK_TCK` |
| COLL_WEIGHTS_MAX | 在本地定义文件中可以赋予LC_COLLATE顺序关键字项的最大权重数 | `_SC_COLL_WEIGHTS_MAX` |
| DELAYTIMER_MAX | 定时器最大超限运行次数 | `_SC_DELAYTIMER_MAX` |
| HOST_NAME_MAX | `gethostname`函数返回的主机名最大长度 | `_SC_HOST_NAME_MAX` 
| IOV_MAX | `readv`或`writev`函数可以使用最多的iovec结构的个数 | `_SC_IOV_MAX` |
| LINE_MAX | 实用程序输入行的最大长度 | `_SC_LINE_MAX` |
| LOGIN_NAME_MAX | 登录名的最大长度 | `_SC_LOGIN_NAME_MAX` |
| NGROUPS_MAX | 每个进程同时添加的最大进程组ID数 | `_SC_NGROUPS_MAX` |
| OPEN_MAX | 每个进程最大打开文件数 | `_SC_OPEN_MAX` |
| PAGESIZE | 系统存储页长度(字节数) | `_SC_PAGESIZE` |
| PAGE_SIZE | 系统存储页长度(字节数) | `_SC_PAGE_SIZE` |
| RE_DUP_MAX | 但使用间隔表示法\\{m,n\\}时,函数`regexec`和`regcomp`允许的基本正则表达式重复发生次数 | `_SC_RE_DUP_MAX` |
| RTSIG_MAX | 为应用程序预留的实时信号的最大个数 | `_SC_RTSIG_MAX` |
| SEM_NSEMS_MAX | 一个进程可使用的信号量的最大个数 | `_SC_SEM_NSEMS_MAX` |
| SEM_VALUE_MAX | 信号量的最大值 | `_SC_SEM_VALUE_MAX` |
| SIGQUEUE_MAX | 一个进程可排队信号的最大个数 | `_SC_SIGQUEUE_MAX` |
| STREAM_MAX | 一个_SC_STREAM_MAX进程在任意给定时刻标准IO流的最大个数。如果定义,必须与FOPEN_MAX有相同值 | `_SC_STRAM_MAX` |
| SYMLOOP_MAX | 在解析路径名时可遍历的符号链接数 | `_SC_SYMLOOP_MAX` |
| TIMER_MAX | 每个进程的最大定时器个数 | `_SC_TIMER_MAX` |
| TTY_NAME_MAX | 终端设备名长度,包括终止null字节 | `_SC_TTY_NAME_MAX` |
| TZNAME_MAX | 时区名中的最大字节数 | `_SC_TZNAME_MAX` |

- pathconf函数
```cpp
/**
 * @brief 获取指定路径__path和限制名__name所对应的实时限制值
 * @param __path 路径
 * @param __name 限制名
 * @exception -1 限制值是不确定的
 * @exception -1, 置errno为EINVAL __name参数不是一个合适的常量
 */
long pathconf(const char *__path, int __name);
```
- fpathconf函数
```cpp
/**
 * @brief 获取指定文件描述符fd和限制名__name所对应的实时限制值
 * @param __fd 文件描述符
 * @param __name 限制名
 * @exception -1 限制值是不确定的
 * @exception -1, 置errno为EINVAL __name参数不是一个合适的常量
 */
long fpathconf(int __fd, int __name);
```
| 限制名 | 说明 | name参数 |
| ----------- | ----------- | ----------- |
| FILESIZEBITS | 以带符号整型值表示在指定目录中允许的普通文件最大长度所需的最小位数 | `_PC_FILESIZEBITS` |
| LINK_MAX | 文件链接数的最大值 | `_PC_LINK_MAX` |
| MAX_CANON | 终端规范输入队列的最大字节数 | `_PC_MAX_CANON` |
| MAX_INPUT | 终端输入队列可用空间的字节数 | `_PC_MAX_INPUT` |
| NAME_MAX | 文件名的最大字节数(不包括终止字符null) | `_PC_NAME_MAX` |
| PATH_MAX | 相对路径名的最大字节数,包括终止字符null | `_PC_PATH_MAX` |
| PIPE_BUF | 能原子的写到管道的最大字节数 | `_PC_PIPE_BUF` |
| SYMLINK_MAX | 符号链接的字节数 | `_PC_SYMLINK_MAX` |