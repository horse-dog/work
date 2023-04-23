## 1 unix体系结构
| 概念 | 含义 |
| ----------- | ----------- |
| 内核 | 操作系统软件 |
| 系统调用 | 内核的接口 |
| 库函数 | 系统调用的封装 |
| 应用程序 | 调用库函数或直接进行系统调用运行的程序 |
| shell | 特殊的应用程序 |

## 2 Linux常见shell
| 名称 | 路径 | Linux |
| ----------- | ----------- | ----------- |
| Bourne shell | /bin/sh | · |
| Bourne-again shell | /bin/bash | · |
| C shell | bin/csh | 可选的 |
| Korn shell | bin/ksh | 可选的 |
| TENEX shell | bin/tcsh | 可选的 |

## 3 文件和目录
| 概念 | 含义 |
| ----------- | ----------- |
| 文件系统 | 目录和文件组成的一个层次结构，目录的起点称为`根`，其名字是一个字符 `\` |
| 目录 | 一个包含许多目录项的文件 |
| 文件名 | 目录中的各个名字称为文件名,`.` 指当前目录, `..` 指父目录 |
| 路径名 | 一个或多个以斜线分隔的文件名序列构成路径名 |
| 绝对路径 | 从盘符开始的路径，形如 `C:\windows\system32\cmd.exe` |
| 相对路径 | 从当前路径开始的路径 |
| 工作目录 | 每个进程都有一个工作目录,进程可以用`chdir`函数更改工作目录 |

### 3.1 数据结构
- 目录
```cpp
typedef struct __dirstream DIR;
```
- 目录项
```cpp
struct dirent
  {
#ifndef __USE_FILE_OFFSET64
    __ino_t d_ino;          /* inode number  索引节点号 */
    __off_t d_off;          /* offset to this dirent 在目录文件中的偏移 */
#else
    __ino64_t d_ino;
    __off64_t d_off;
#endif
    unsigned short int d_reclen;    /* length of this d_name 文件名长 */
    unsigned char d_type;           /* the type of d_name 文件类型 */
    char d_name[256];		        /* file name (null-terminated) 文件名，最长256字符 */
  };
```
### 3.2 相关函数
- opendir: 打开目录
```cpp
/**
 * @brief 打开目录
 * @param __name 目录路径
 * @return DIR指针
 * @exception NULL
 */
DIR *opendir(const char *__name);
```
- readdir: 遍历目录
```cpp
/**
 * @brief 遍历目录
 * @param __dirp DIR指针
 * @return dirent指针
 * @exception NULL
 */
struct dirent *readdir(DIR *__dirp);
```
- closedir: 关闭目录
```cpp
/**
 * @brief 关闭目录
 * @param __dirp DIR指针
 * @return 0
 * @exception -1
 */
int closedir (DIR *__dirp);
```
### 3.3 ls实现
```cpp
#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    DIR             *dp;
    struct dirent *dirp;
    
    if (argc != 2)
        return -1;
    if ((dp = opendir(argv[1])) == NULL)
        return -2;
    while ((dirp = readdir(dp)) != NULL)
        printf("%s\n", dirp->d_name);
    closedir(dp);
    return 0;
}
```
## 4. 输入和输出
| 概念 | 含义 |
| ----------- | ----------- |
| 文件描述符 | 通常是一个小的非负整数，内核用以标识一个特定进程正在访问的文件 |
| 标准输入、标准输出和标准错误 | 每当运行一个新程序时，所有的 shell 都为其打开3个文件描述符，即标准输入、标准输出以及标准错误。如果不做特殊处理，则这3个描述符都链接向终端 |
| 不带缓冲的 I/O | 函数 `open`、`read`、`write`、`lseek` 以及 `close` 提供了不带缓冲的 I/O。这些函数都使用文件描述符。 |
| 标准 I/O | 标准 I/O 函数为那些不带缓冲的 I/O函数提供了一个带缓冲的接口 |

### 4.1 标准输入输出重定向命令
```cpp
/* The value returned by fgetc and similar functions to indicate the
   end of the file.  */
#define EOF (-1)

/* Standard streams.  */
extern FILE *stdin;	    /* Standard input stream.  */
extern FILE *stdout;    /* Standard output stream.  */
extern FILE *stderr;    /* Standard error output stream.  */

/* C89/C99 say they're macros.  Make them happy.  */
#define stdin stdin

#define stdout stdout

#define stderr stderr

/* Standard file descriptors.  */
#define	STDIN_FILENO	0	/* Standard input.  */

#define	STDOUT_FILENO	1	/* Standard output.  */

#define	STDERR_FILENO	2	/* Standard error output.  */
```
- `>` 和 `1>` ，`>` 和 `2>` 的区别？
    - `>` 是 `1>` 的简写，习惯会将 1 省去，直接写成 `>`
    - `>` 是将正确信息输出,`2>` 是将错误信息输出, 1 和 2 对应了文件描述符
- `>` 和 `>>` 的区别
    - `>` 以覆盖的方式输出,`>>` 以追加的方式输出
- 在书写 `2>` 或者 `2>>` 时，数字和符号中间不能有空格
- `2>&1` 将错误返回值传递给1输出通道, &1表示1输出通道
- `1>&2` 将正确返回值传递给2输出通道，&2表示2输出通道

| 类型 | 符号 | 作用 |
| ----------- | ----------- | ----------- |
| 标准输入重定向 | `command < file` | 将file文件作为command的输入 |
|  | `command < file1 > file2` | 将file1作为command输入，command处理结果输出到file2 |
|  | `command << END`| 从标准输入(键盘)读取数据，直到遇见分界符END才停止。分界符是可以是任意的，用户自己定义 |
| 标准输出重定向 | `command > file` | 以覆盖方式将内容输出到file文件 |
|  | `command >> file` | 以追加方式将内容输出到file文件 |
| 标准错误重定向 | `command 2> file` | 以覆盖方式将错误输出到file文件 |
|  | `command 2>> file` | 以追加方式将错误输出到file文件 |
| 正确输出和错误信息同时保存 | `command > file 2>&1` | 以覆盖方式将正确输出和错误信息输出到file文件 |
|  | `command >> file 2>&1` | 以追加方式将正确输出和错误信息输出到file文件 |
|  | `command > file1 2> file2` | 略 |
|  | `command >> file1 2>> file2` | 略 |
|  | `command > file 2> file` | __不推荐__, 该写法会导致file被打开两次,引起资源竞争,stdout和stderr会相互覆盖 |
|  | `command >> file 2>> file` | __不推荐__, 该写法会导致file被打开两次,引起资源竞争,stdout和stderr会相互覆盖 |

### 4.2 无缓冲IO函数
- read: 读取函数
```cpp
typedef long ssize_t

/**
 * @brief 从指定文件描述符读取数据到缓冲区
 * @param __fd 文件描述符
 * @param __buf 缓冲区指针
 * @param __nbytes 读取的字节数
 * @return 实际读取的字节数
 * @return 0 EOF
 * @exception -1 errors
 */
ssize_t read(int __fd, void *__buf, size_t __nbytes);
```
- write: 写入函数
```cpp
typedef long ssize_t

/**
 * @brief 将缓冲区数据写入到指定文件描述符
 * @param __fd 文件描述符
 * @param __buf 缓冲区指针
 * @param __n 需要写入的字节数
 * @return 实际写入的字节数
 * @exception -1 errors
 */
ssize_t write(int __fd, const void *__buf, size_t __n);
```

### 4.3 标准IO函数
- getc: 字符读取函数
```cpp
/**
 * @brief 从指定的流 stream 获取下一个字符（一个无符号字符），并把位置标识符往前移动
 * @param __stream 指向FILE对象的指针
 * @return u_char 强制转换为 int 的形式返回读取的字符
 * @exception EOF 到达文件末尾或发生读错误
 */
int getc(FILE *__stream);
```
- putc: 字符输出函数
```cpp
/**
 * @brief 把参数 char 指定的字符（一个无符号字符）写入到指定的流 stream 中，并把位置标识符往前移动
 * @param __c 这是要被写入的字符。该字符以其对应的 int 值进行传递
 * @param __stream 这是指向 FILE 对象的指针
 * @return 写入的字符, u_char 强制转换为 int
 * @exception EOF 发生错误
 */
int putc(int __c, FILE *__stream);
```
- ferror: 检测文件流是否发生了错误
```cpp
/**
 * @brief 检测文件流是否发生了错误
 * @param __stream 指向FILE对象的指针
 * @return 0(文件流未出错) | !0(文件流出错)
 */
int ferror(FILE *__stream);
```
### 4.4 cp 实现-无缓冲IO
```cpp
#include <stdio.h>
#include <unistd.h>
#define BUFFSIZE 4096

int main() {
    int  n;
    char buf[BUFFSIZE];
    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n)
            return -1;
    if (n < 0)
        return -2;
    return 0;
}
```

### 4.5 cp 实现-标准IO,字符读写
```cpp
#include <stdio.h>

int main() {
    int c;
    while ((c=getc(stdin)) != EOF)
        if (putc(c, stdout) == EOF)
            return -1;
    if (ferror(stdin))
        return -2;
    return 0;
}
```

## 5 程序和进程
| 概念 | 含义 |
| ----------- | ----------- |
| 程序 | 一个存储在磁盘上某个目录的可执行文件。内核使用`exec`函数将程序读入内存并执行程序 |
| 进程 | 程序的执行实例 |
| PID | 进程ID,一个非负整数。在进程中可以通过`getpid()`系统调用获取自身PID |
| 线程 | 一个进程内的所有线程共享同一地址空间、文件描述符、栈以及与进程相关的属性 |

## 6 出错处理
- errno
```cpp
extern int *__errno_location (void);
# define errno (*__errno_location ())
```
- strerror函数
```cpp

#  define __THROW	__attribute__ ((__nothrow__ __LEAF))

/**
 * @brief 返回错误码对应的字符串
 * @param __errnum 错误号
 * @return 错误字符串
 */
extern char *strerror (int __errnum) __THROW;
```
- perror函数
```cpp
/**
 * @brief 打印错误码对应的字符串
 * @param __s 提示字符串
 */
extern void perror (const char *__s) {
    /* possible implementation */
    printf("%s: %s\n", __s, strerror(errno));
    exit(-1);
}
```

## 7 用户标识
- 用户名与用户ID
- 组与组ID
- 附属组
- getuid(): 获取用户ID
- getgid(): 获取组ID

## 8 信号
```cpp
typedef void (*__sighandler_t) (int);
extern __sighandler_t signal(int __sig, __sighandler_t __handler);
```

## 9 时间
### 9.1 日历时间
- 自 __1970年1月1日00:00:00__ 起到某时刻的秒数，通过`time_t`类型保存这种时间值。

### 9.2 进程时间
- 用于表示进程使用CPU时间资源。通过`clock_t`类型保存这种时间值。
- UNIX系统为每个系统维护了三个进程时间值。

| 概念 | 含义 |
| ----------- | ----------- |
| 时钟时间 | 进程运行的总时间。时钟时间 = 阻塞时间 + 就绪时间 + 运行时间。 |
| 用户CPU时间 | 进程获得了CPU资源以后，在用户态执行的时间。 |
| 系统CPU时间 | 进程获得了CPU资源以后，在内核态的执行时间。 |
| CPU时间 | CPU时间 = 用户CPU时间 + 系统CPU时间 |

## 10 系统调用与库函数
| 概念 | 含义 |
| ----------- | ----------- |
| 系统调用 | 访问内核的接口 |
| 库函数 | 系统调用的封装,提供复杂功能 |