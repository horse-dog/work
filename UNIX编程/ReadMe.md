#### 参考资料：UNIX环境高级编程

#### UNIX基础知识

- 了解UNIX的体系结构

#### UNIX标准及实现

- 了解常用标准：ISO C、POSIX、Single UNIX Specification

- 了解常见的UNIX系统实现：Linux、FreeBSD、MACOSX等

- 了解库函数 *sysconf* 可以获取系统执行的配置信息

#### 文件IO

- 熟悉函数open、openat及常用oflag参数

- 熟悉函数creat及其等效的open调用：open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);

- 熟悉函数close

- 熟悉函数lseek

- 熟悉函数read及返回情况

  - 大于0：实际读取的字节数

  - 等于0：读取到了EOF

  - -1：出错

- 熟悉函数write及返回情况

  - 大于等于0：实际写入的字节数

  - -1：出错

- 熟悉UNIX文件共享时的内核数据结构：文件描述符表、打开文件表、i节点

- 了解文件的原子操作

  - 了解追加操作先lseek到文件尾部再写入和open时指定O_APPEND的区别（前者是非原子操作）

  - 了解原子读写pread和pwrite：原子性的完成设置偏移量和读/写两个操作

- 熟悉函数dup和dup2

- 了解函数sync、fsync、fdatasync

  - void sync()：将所有修改过的块缓冲区排入写队列,然后返回,它并不等待实际写磁盘操作结束

  - int fsync(int fd)：使描述符fd同步写

  - int fdatasync(int fd)：使描述符fd同步写数据

- 熟悉函数fcntl

- 了解函数ioctl

#### 文件和目录

- 了解文件信息结构stat结构体：包含有成员st_mode、st_ino、st_uid、st_gid、st_size等文件元信息

- 熟悉stat函数用于获取文件信息结构

- 了解stat函数的变体fstat、lstat、fstatat

- 了解常见的文件类型，以及如何判断一个文件是什么类型

- 了解常用的st_mode标记

- 了解st_mode中**S_ISUID**和**S_ISGID**标志的功能

- 熟悉函数access、faccessat用于测试进程按实际用户ID和实际组ID对文件的访问权限

- 熟悉umask函数设置文件模式创建屏蔽字

- 熟悉函数chmod、fchmod、fchmodat修改文件的访问权限

- 熟悉函数chown、fchown、fchownat、lchown修改文件的用户ID和组ID

- 熟悉函数truncate用于文件截断

- 熟悉函数link、linkat用于创建硬链接

- 熟悉函数unlink、unlinkat用于删除目录项

- 熟悉函数remove用于删除文件或目录：对于文件，该函数等效于unlink；对于目录，该函数等效于rmdir

- 熟悉函数symlink、synlinkat用于创建符号链接

- 熟悉函数readlink、readlinkat用于读取符号链接

- 熟悉stat结构中的三个时间戳st_atime、st_mtime、st_ctime的含义

- 熟悉函数mkdir、mkdirat用于创建目录

- 熟悉函数rmdir用于删除目录

- 熟悉函数opendir、fdopendir用于打开目录

- 熟悉函数readdir用于遍历目录、rewinddir用于回绕目录、closedir用于关闭目录

- 熟悉函数chdir、fchdir用于修改进程的工作目录

- 熟悉函数getcwd用于获取当前工作目录

#### 标准IO库

#### 系统数据文件和信息

#### 进程环境

#### 进程控制

#### 进程关系

#### 信号

#### 线程

#### 线程控制

#### 守护进程

#### 高级IO

#### 进程间通信

#### 网络IPC: 套接字

#### 高级进程间通信