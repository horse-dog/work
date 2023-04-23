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

- 了解标准IO的设计初衷：减少read/write系统调用的频率

- 了解字节定向和宽定向，以及如何改变流的定向

  - `freopen`函数清除一个流的定向

  - `fwide`函数可用于设置流的定向

- 了解三种缓冲模式：全缓冲、行缓冲、不带缓冲

- 熟悉setbuf、setvbuf用于定义流的缓冲区和缓冲模式

- 熟悉fflush函数用于强制冲洗一个流

- 熟悉函数fopen用于打开流

- 熟悉函数freopen把一个新的文件名与给定的打开的流关联，同时关闭流中的旧文件，若流已经定向，则使用 freopen 将清除该定向

- 熟悉函数fdopen用于从文件描述符打开文件流

- 熟悉函数fclose用于关闭流

- 熟悉各种文件访问模式："r"、"w"、"a"、"r+"、"w+"、"a+"

- 熟悉函数getc、fgetc、getchar及返回值

  - 大于等于0：读取到的字符（这里被向上转换为了int）

  - -1：读取到了文件末尾（EOF）或者发生了错误，至于到底是读到了末尾还是发生了错误，需要使用ferror函数或feof函数判断

- 熟悉函数ferror检查文件流是否出错，feof检查文件流是否到达文件末尾，clearerr清除文件流的出错标志和文件结束标志

- 熟悉函数putc、fputc、putchar向流中写入字符，熟悉返回值

  - 大于等于0：刚刚写入的字符

  - -1：出错

- 熟悉函数fgets和gets用于读取一行，了解gets的安全问题，熟悉返回值

  - 非NULL：buffer的指针

  - NULL：到达文件尾部或出错

- 熟悉函数fputs和puts用于写入一行，熟悉返回值

  - 大于等于0：写入的字节数

  - -1：出错

- 熟悉函数fread和fwrite用于流的二进制IO

- 了解流的定位相关函数：ftell/ftello/fgetpos、fseek/fseeko/fsetpos、rewind

- 了解printf函数族：printf、fprintf、dprintf、sprintf、snprintf、vprintf、vfprintf、vdprintf、vsprintf、vsnprintf

- 了解scanf函数族：scanf、fscanf、sscanf、vscanf、vfscanf、vssacnf

- 了解函数fileno用于获取一个流的文件描述符

- 了解 tmpnam、tmpfile、mkdtemp、mkstemp用于创建临时文件和目录

- 熟悉内存流的概念和相关函数：fmemopen、open_memstream、open_wmemstream

#### 系统数据文件和信息

- 熟悉时间相关的结构：timeval、timespec、time_t、timezone、tm

- 熟悉时间获取函数：gettimeofday(获取timeval)、clock_gettime(获取timespec)、time(获取time_t)

- 了解time_t和tm的转换

#### 进程环境

- 了解exit、_exit、_Exit的区别、了解函数atexit注册终止处理程序

- 熟悉main函数的命令行参数

- 熟悉环境变了表及获取方式

- 熟悉C语言程序的存储空间布局

- 熟悉动态存储空间分配函数：malloc、calloc、realloc、free

- 熟悉跳转函数setjmp和longjmp

- 了解getrlimit和setrlimit用于读写进程的资源限制

#### 进程控制

- 熟悉函数族：getpid、getppid、getuid、geteuid、getgid、getegid

- 熟悉函数fork的功能和使用、熟悉写时复制

- 熟悉函数vfork的功能和使用，以及与fork的区别

- 熟悉函数exit

- 熟悉函数wait和waitpid、了解waitpid可以设置为非阻塞等待、熟悉僵尸进程的定义

- 了解函数wait3和wait4在wait和waitpid的基础上可以获取更多结束的子进程的信息

- 熟悉exec函数族：execl、execv、execle、execve、execlp、execvp、fexecve

- 熟悉函数setuid用于更改进程的实际用户ID和有效用户ID、函数setgid用于设置进程的实际用户ID和有效用户ID

- 熟悉函数seteuid用于更改进程的有效用户ID，setegid用于更改进程的有效组ID

- 熟悉解释器文件的格式

- 熟悉函数system

- 了解进程会计

- 了解getlogin函数用于获取登录名

- 熟悉nice函数用于获取或更改当前进程对CPU的友好值，友好值用于进程调度

- 熟悉getpriority函数用于获取指定进程（组）的友好值

- 熟悉setpriority函数用于更改指定进程（组）的友好值

#### 进程关系

- 熟悉父进程、僵尸进程概念

- 熟悉如何在父进程继续运行的情况下既能避免子进程成为僵尸进程，又能实现不阻塞等待子进程运行完毕

  - 方式一：捕捉异步信号SIGCHLD、在信号处理函数中调用wait或waitpid（wait和waitpid是可重入的）

  - 方式二：fork两次以避免僵尸进程（src/08/8-8.c）

- 熟悉进程组、前台进程组、后台进程组、会话、控制终端的概念

- 熟悉shell作业控制

- 熟悉孤儿进程组的定义

#### 信号

- 熟悉signal函数的使用

- 熟悉信号对低速系统调用的中断机制

- 熟悉可重入函数、线程安全函数、异步信号安全函数的概念

- 熟悉kill函数、raise函数的使用

- 熟悉pthread_kill函数把信号发送给线程

- 熟悉alarm函数、pause函数的使用

- 熟悉信号集的定义和设置api

- 熟悉sigprocmask函数用于设置进程的信号屏蔽字

- 熟悉pthread_sigmask函数用于设置线程的信号屏蔽字，线程的信号屏蔽字是thread_local的

- 熟悉sigwait函数的使用

  - 假设线程想要等待的信号集为set，则先调用pthread_sigmask阻塞s

  - 然后调用sigwait(set, num)来等待s中的任一信号发生，num用于存储sigwait返回时set中的等待到的信号的数量

  - sigwait函数会原子地取消信号集set中信号的阻塞状态，然后使得调用线程休眠，直到有 set 中包含的信号被递送，此时该函数将会返回，在返回之前，sigwait 函数将会恢复线程中的信号屏蔽字

  - 熟悉如何使用pthread_sigmask和sigwait函数在多线程环境下将进程对信号的异步处理机制转换为多线程环境下对信号的同步处理

- 熟悉sigpending函数用于获取未决信号集

- 熟悉sigaction函数用于设置信号处理程序

  - 熟悉 `sa_mask` 说明一个信号集，在调用信号捕捉函数之前，这一信号集要**追加**到进程的信号屏蔽字中。信号捕捉函数返回时，恢复原先的信号屏蔽字。这样，在调用信号处理程序时就能阻塞某些信号。此外，无论sa_mask中是否含有本信号，进入信号捕捉函数中时，本信号一定都是被屏蔽的。signal函数也是如此。

  - 了解sigaction函数可以设置额外的选项：例如中断系统调用是否自动重启

- 了解sigsetjmp和siglongjmp用于跳转，并且可以恢复信号屏蔽字。

- 熟悉sigsuspend函数设置信号屏蔽字，然后使得进程休眠，该函数是一个原子操作。该函数捕获除了 `sigmask` 参数指向的信号集之外的任意信号，即使该信号在 `sigsuspend` 调用之前在进程的信号屏蔽字中也是如此，如果捕捉到一个信号而且从该信号处理程序返回，则 `sigsuspend` 返回，并且该进程的信号屏蔽字设置为调用 `sigsuspend` 之前的值。

- 熟悉abort函数的使用

- 熟悉sleep函数，了解nanosleep、clock_nanosleep函数

#### 线程

- 见并发编程

#### 线程控制

- 见并发编程

#### 守护进程

- 熟悉守护进程的特征

  - 守护进程时生存期长的一种进程，它们常常在系统引导装入时启动，仅在系统关闭时终止。

  - 没有控制终端，运行在后台。

- 熟悉两次fork实现守护进程：src/13/13-1.c

#### 高级IO

- 熟悉阻塞IO、非阻塞IO的定义

- 熟悉如何使用fcntl设置文件描述符的IO为非阻塞的

- 了解记录锁的定义和fcntl函数对记录锁的控制功能

- 熟悉IO复用系统调用：select、poll、epoll

- 了解基于信号SIGIO、SIGURG的IO方式

- 熟悉异步IO的定义和原理、了解POSIX异步IO

- 熟悉分散读和聚集写readv、writev

- 熟悉mmap的作用和实现原理

#### 进程间通信

- 熟悉管道的功能，pipe函数的使用，数据的流向：1 -> 0

- 熟悉当写一个读端已被关闭的管道时，产生信号 `SIGPIPE`。

- 了解fcntl函数可以设置管道的容量

- 熟悉函数popen、pclose的使用

- 了解FIFO（命名管道）

- 了解XSI IPC

  - 键key（类比文件名）
  
  - IPC描述符（类比文件描述符）

  - 信号量集的semget、共享内存的shmget、消息队列的msgget（类比文件IO的open函数）

  - semctl、shmctl、msgctl（类比fcntl函数）

  - semid_ds、shmid_ds、msqid_ds（类比stat结构），三者具有部分相同的字段，这些相同的字段抽取为ipc_perm结构。

#### 网络IPC: 套接字

- 见网络编程

#### 高级进程间通信

- 熟悉socketpair函数的功能和使用

- 熟悉UNIX域套接字