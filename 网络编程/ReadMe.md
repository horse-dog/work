#### 参考资料：Linux高性能服务器编程（游双）

#### TCP/IP协议族

- 熟悉OSI七层模型：自上而下依次为应用层、表示层、会话层、传输层、网络层、数据链路层、物理层

- 熟悉TCP/IP四层模型：自上而下依次为应用层、传输层、网络层、数据链路层

- 熟悉ping程序、telnet协议、OSPF协议、DNS协议、TCP协议、UDP协议、ICMP协议、IP协议、ARP协议、RARP协议之间的依赖关系

- 熟悉数据链路层的功能，熟悉ARP协议、RARP协议的功能和实现原理（先广播、后单播）

- 熟悉MAC帧的结构：| 目的物理地址(6字节) | 源物理地址(6字节) | 类型(2字节) | 数据(46~1500字节) | CRC(4字节) |

- 熟悉网络层的功能，熟悉IP协议的功能，熟悉ICMP协议的功能

- 熟悉传输层的功能，熟悉TCP协议、UDP协议的特性和功能

- 熟悉应用层的功能，了解常见的应用层协议和应用

- 深刻理解计算机网络的封装思想

- 深刻理解计算机网络的分用思想，熟悉常见的分用场景

  - ARP、RARP、IP协议通过MAC头部的类型字段分用

  - ICMP、TCP、UDP通过IP数据报头部的8位协议字段分用

  - 不同的应用程序通过TCP/UDP的端口号分用

- 熟悉DNS协议的功能、原理、递归和迭代的实现方式

#### IP协议详解

- 熟悉IP头部的组成，熟悉各个头部字段的作用

- 熟悉IP分片、熟悉MTU的含义

- 熟悉IP路由的基本方式

- 了解路由表更新的算法：RIP、OSPF、BGP

- 了解主机重定向的含义和作用

- 熟悉三个私有IP段

- 熟悉NAT的工作原理

#### TCP协议详解

- 熟悉TCP服务的特点

- 熟悉TCP头部的结构，各个字段的含义

- 熟悉TCP连接正常建立和关闭的三次握手与四次挥手，熟悉MSL的概念，熟悉TCP状态机的各个状态以及第四次挥手的作用

- 熟悉复位报文段的作用

- 熟悉TCP超时重传机制

- 熟悉冗余ACK的概念和快速重传机制

- 了解常见的两种TCP数据流：交互式数据流（追求实时性）、成块数据流（追求传输效率）

- 熟悉TCP的拥塞控制：Nagle算法、接收窗口、拥塞窗口、慢开始、拥塞避免算法、快重传、快恢复算法

- 了解TCP keepalive选项的保活方式，了解客户端断电和进程崩溃两种情况下服务器端的连接情况：https://zhuanlan.zhihu.com/p/390939380

#### TCP/IP通信案例

- 熟悉HTTP代理服务器的工作原理：正向代理、反向代理、透明代理

- 熟悉HTTP请求头和HTTP响应头的组成

- 了解常见的HTTP状态码及其含义

- 熟悉Cookie的工作原理

#### Linux网络编程基础

- 熟悉大小端、熟悉字节序的转换api：htonl、htons、ntohl、ntohs

- 熟悉通用socket地址结构和专用socket地址结构（IPv4、UNIX域）

- 熟悉IP地址字符串表示与数值表示的转换：inet_pton转字符串为网络序、inet_ntop转网络序为字符串

- 熟悉socket函数的使用，创建TCP/UDP连接的方式

- 熟悉bind函数的使用

- 熟悉listen函数的使用，熟悉backlog机制

- 熟悉accpet函数的使用

- 熟悉connect函数的使用

- 熟悉close函数、shutdown函数的使用，熟悉二者的区别

- 熟悉recv函数，熟悉常用标志：MSG_OOB、熟悉返回值

  - 大于0：从内核接收缓冲区读取到的的字节数

  - 等于0：对端关闭连接

  - -1：出错

- 熟悉send函数，熟悉常用标志：MSG_OOB、熟悉返回值

  - 大于等于0：写入内核发送缓冲区的字节数

  - -1：出错

- 了解UDP数据读写函数：recvfrom和sendto

- 了解通用数据读写函数：recvmsg和sendmsg

- 了解sockatmark函数可以用于判断sockfd是否处于带外标记

- 了解getsockopt函数和setsockopt函数用于读写sockfd的属性，了解常用属性：SO_REUSEADDR、SO_LINGER、SO_RCVBUF、SO_SNDBUF、SO_RCVTIMEO、SO_SNDTIMEO、IP_TOS、IP_TTL

#### 高级I/O函数

- 熟悉pipe函数使用、数据流向

- 熟悉socketpair函数的特点和使用

- 熟悉dup函数和dup2函数

- 熟悉readv函数和writev函数用于分散读和聚集写

- 熟悉sendfile函数用于直接在内核中在描述符直接拷贝数据

- 熟悉mmap函数的使用和实现原理

- 了解splice函数的使用和功能

- 了解tee函数的使用和功能

- 熟悉fcntl函数的使用和功能

#### Linux服务器编程规范

- 熟悉Linux内核日志和用户日志的实现机制

- 了解syslog、openlog、setlogmask、closelog函数的功能。

  - syslog生成系统日志

  - openlog控制syslog的输出方式

  - setlogmask设置日志掩码

  - closelog关闭日志功能

- 了解获取/设置用户信息的各个系统api的功能：getuid、geteuid、getgid、getegid、setuid、seteuid、setgid、setegid

- 熟悉进程组、进程组长、会话、会话话长等概念，熟悉setpgid只能设置自己或子进程的进程组，熟悉setsid只能由非进程组长调用

- 了解系统资源限制的api：getrlimit、setrlimit

- 熟悉工作目录相关函数：getcwd、chdir、chroot

- 熟悉daemon函数的使用，熟悉daemon函数的实现方式：两次fork，setsid，012指向null...

#### 高性能服务器程序框架

- 熟悉C/S模型和P2P模型各自的定义和特点

- 熟悉五种IO模型的定义：阻塞IO、非阻塞IO、信号IO、IO多路复用、异步IO

- 熟悉Reactor和Proactor两种服务器模型的定义和区别

- 熟悉半同步/半反应堆模型

- 熟悉半同步/半异步模型，熟悉其相较于半同步/半反应堆模型的优点：每个工作线程可以管理多个客户端

- 熟悉领导者/追随者模型

- 熟悉有限状态机的定义和使用场景

- 熟悉池化技术的作用，熟悉常见的池化：进程池、线程池、内存池、数据库连接池

- 熟悉零拷贝技术的定义和使用场景

- 理解上下文切换和加锁保护对服务器性能的影响

#### I/O复用

- 熟悉select系统调用

- 熟悉poll系统调用

- 熟悉epoll系统调用，熟悉ET和LT模式的区别和优缺点

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

- 熟悉统一事件源的设计思想

- 熟悉网络编程的相关信号：SIGHUP、SIGPIPE、SIGURG

#### 定时器

- 熟悉socket选项SO_RCVTIMEO和SO_SNDTIMEO对各个读写函数的影响，注意accept也是一个读函数，connect也是一个写函数，熟悉这些函数超时后的行为

- 熟悉定时器的定义、功能

- 熟悉时间轮的设计思路，具有手写多级时间轮的能力

- 熟悉时间堆的设计思路，具有手写时间堆的能力

#### 多进程编程

- 熟悉fork系统调用

- 熟悉exec函数族

- 熟悉wait函数和waitpid函数的使用，了解waitpid函数可以等待进程、进程组等等特定集合，了解waitpid函数可以非阻塞地等待

- 熟悉僵尸进程的定义和处理方式

  - 异步信号方式：捕捉SIGCHLD信号，在处理程序中wait

  - 两次fork方式：将子进程托管给init进程

- 了解信号量、消息队列、共享内存的XSI IPC实现

- 了解IPC命令：ipcs、ipcrm

- 了解可以通过UNIX域套接字将子进程的文件描述符传递给父进程

#### 多线程编程

- 熟悉linux多线程编程api

- 熟悉pthread_atfork的作用

- 熟悉多线程信号相关api：pthread_sigmask、sigwait、pthread_kill

#### 进程池和线程池

- 熟悉进程池的实现方式，能手写简单的进程池

- 熟悉线程池的实现方式，能手写简单的线程池

#### 服务器调制、调试和测试

- 了解用户级、系统级最大文件描述符的概念，了解资源限制可以更改

- 了解TCP同步标签可用于防止洪泛SYN

- 熟悉GDB基本命令的使用

- 熟悉多线程、多进程程序的调试

- 熟悉GDB断点的实现原理

- 熟悉coredump的调试

#### springsnail

<img src="img/16.png" style="zoom:100%" />

- master进程负责进行反向代理

- 每个worker进程负责与一个对应的逻辑服务器通信

- 每个worker进程与对应的逻辑服务器通信的连接数量有限制，此时这种连接成为一种资源，称之为conn，每个conn都有一个client buffer，一个server buffer，还有一个套接字srvfd，可用于与remote server通信

- 每个worker进程的conn资源通过manager进行管理，manager维护一个正在使用的conn链表和一个空闲的conn链表

- 在fork出多个worker进程之前，listenfd已经建立并且被监听，因此所有worker进程都拥有listenfd实例，但只有master进程在epoll事件里注册了listenfd的可读事件，因此客户连接请求只会被master进程接收到

- 当用户向反向代理服务器（master）发起连接请求时，master选择一个负载最小的（即正在使用的conn数量占该worker总共拥有的conn数量最小的）worker进程，使用管道向这个进程发送一个字节，这个字节的内容不重要，只是用于通知该worker去accept一个连接，因为worker进程拥有listenfd实例，所以可以通过listenfd去accept这个连接。此时，客户端和该worker的通信链路建立完成。

- 客户端最终的要求是和remote server进行通信，remote server可以有多个，它们向客户提供具体的服务。

- 客户端与一个worker的连接建立后，该worker取出一个空闲的conn，该conn负责与remote server进行通信。这样，conn就成为了客户与remote server联系的中转。

- 当客户发送数据时，worker读取内核接收缓冲区到client buffer，然后worker发送client buffer的内容到remote server，这样，客户的数据就能被remote server接收到。

- remote server接收到客户数据后，进行回应，回应的数据会被发送到这个worker进程的这个conn的srvfd，然后该worker读取这个srvfd的内核接收缓冲区到server buffer中，然后worker发送server buffer的内容到客户，这样，remote server回应的数据就能被递送到客户了。
