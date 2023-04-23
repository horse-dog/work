## 1 套接字描述符
```cpp
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
// 返回值：若成功，返回文件（套接字）描述符；若出错，返回-1
```

| 域 | 描述 |
| -- | -- |
| `AF_INET` | `IPv4` 因特网域 |
| `AF_INET6` | `IPv6` 因特网域 |
| `AF_UNIX` | `UNIX` 域 |
| `AF_UPSPEC` | 未指定 |

| 类型 | 描述 |
| -- | -- |
| `SOCK_DGRAM` | 固定长度的、无连接的、不可靠的报文传递 |
| `SOCK_RAW` | `IP` 协议的数据包接口（在 `POSIX.1` 中为可选） |
| `SOCK_SEQPACKET` | 固定长度的、有序的、可靠的、面向连接的报文传递 |
| `SOCK_STREAM` | 有序的、可靠的、双向的、面向连接的字节流 |

| 协议 | 描述 |
| -- | -- |
| `IPPROTO_IP` | `IPv4` 网际协议 |
| `IPPROTO_IPV6` | `IPv6` 网际协议 |
| `IPPROTO_ICMP` | 因特网控制报文协议（`ICMP`） |
| `IPPROTO_RAW` | 原始 `IP` 数据包协议 |
| `IPPROTO_TCP` | 传输控制协议 |
| `IPPROTO_UDP` | 用户数据报协议 |

- 套接字是通信端点的抽象。正如使用文件描述符访问文件，应用程序使用套接字描述符访问套接字。

- 套接字描述符在 `UNIX` 系统中被当作是一种文件描述符。

- 参数 `domain` (域)指定通信的特征，包括地址格式。各个域都有自己表示地址的格式，而表示各个域的常数都以 `AF_` 开头，意指地址族（address family）。

- 参数 `type` 确定套接字的类型，进一步确定通信特征。

- 参数 `protocol` 通常是 0，表示为给定的域和套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，可以使用 `protocol` 选择一个特定协议。

- 在 `AF_INET` 通信域中，套接字类型 `SOCK_STREAM` 的默认协议是 `TCP`，套接字类型 `SOCK_DGRAM` 的默认协议是 `UDP`。

```cpp
#include <sys/socket.h>
int shutdown(int sockfd, int how);
// 返回值：若成功，返回0；若出错，返回-1
```

- `shutdown` 函数禁止一个套接字的 `I/O`。

- 如果 how 是 SHUT_RD，那么无法从套接字读取数据，如果 how 是 SHUT_WR，那么无法使用套接字发送数据。如果 how 是 SHUT_RDWR，则既无法读取数据，又无法发送数据。

- 能够关闭（close）一个套接字，为何还使用 shutdown 呢？：如果复制（dup）一个套接字，要直到关闭了最后一个引用它的文件描述符才会释放这个套接字。而 shutdown 允许使一个套接字处于不活动状态，和引用它的文件描述符数目无关。

## 2 寻址
### 2.1 字节序
- 小端模式：例如 int 数据 0x12345678，小端序从低位到高位依次为：12，34，56，78。
- 大端模式：例如 int 数据 0x12345678，小端序从低位到高位依次为：78，56，34，12。
- TCP/IP 协议栈使用大端字节序。

```cpp
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32);
// 返回值：以网络字节序表示的32位整数
uint16_t htons(uint16_t hostint16);
// 返回值：以主机字节序表示的16位整数
uint32_t ntohl(uint32_t hostint32);
// 返回值：以网络字节序表示的32位整数
uint16_t ntohs(uint16_t hostint16);
// 返回值：以主机字节序表示的16位整数
```

### 2.2 地址格式
```cpp
#include <arpa/inet.h>
#include <sys/socket.h>
struct sockaddr {
  unsigned char sa_len;     /* total length */
  sa_family_t   sa_family;  /* address family */
  char          sa_data[14];/* variable-length address */
};

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

struct sockaddr_in {
  sa_family_t   sin_family; /* address family */
  in_port_t     sin_port;   /* port number */
  struct in_addr  sin_addr; /* IPv4 address */
  unsigned char sin_zero[8];/* filler */
};

struct sockaddr_in6 {
  sa_family_t   sin6_family;/* address family */
  in_port_t     sin6_port;  /* port number */
  uint32_t    sin6_flowinfo;/* traffic class and flow info */
  struct in6_addr sin6_addr;/* IPv6 address */
  uint32_t    sin6_scope_id;/* set of interfaces for scope */
};

struct in_addr {
  in_addr_t     s_addr;     /* IPv4 address */
};

struct in6_addr {
  uint8_t       s6_addr[16];/* IPv6 address */
};
```

```cpp
#include <arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr,
                      char *restrict str, socklen_t size);
// 返回值：若成功，返回地址字符串指针；若出错，返回NULL
int inet_pton(int domain, const char *restrict str, void *restrict addr);
// 返回值：若成功，返回1；若格式无效，返回0；若出错，返回-1
```

- inet_ntop 将网络字节序的二进制地址转换成文本字符串格式。inet_pton 将文本字符串格式转换成网络字节序的二进制格式。参数 domain 仅支持两个值：AF_INET 和 AF_INET6。

- 对于 inet_ntop，参数 size 指定了保存文本字符串的缓冲区（str）的大小。两个常数用于简化工作：
  - INET_ADDRSTRLEN 定义了足够大的空间来存放一个表示 IPv4 地址的文本字符串。
  - INET6_ADDRSTRLEN 定义了足够大的空间来存放一个表示 IPv6 地址的文本字符串。

- 对于 iet_pton，如果 domain 是 AF_INET，则缓冲区 addr 需要足够大的空间来存放一个32位地址，如果 domain 是 AF_INET6，则需要足够大的空间来存放一个128位地址。

### 2.3 地址查询
```cpp
#include <netdb.h>
struct hostent {
  char   *h_name;     /* name of host */
  char  **h_aliases;  /* pointer to alternate host name array */
  int     h_addrtype; /* address type */
  int     h_length;   /* length in bytes of address */
  char  **h_addr_list;/* pointer to array of network address */
  ...
};

struct hostent *gethostent(void);
// 返回值：若成功，返回指针；若出错，返回NULL
// 如果主机数据库文件没有打开，gethostent会打开它。函数gethostent返回文件中的下一个条目。
void sethostent(int stayopen);
// 函数sethostent会打开文件，如果文件已经被打开，那么将其回绕。当 statopen非0时，调用gethostent后，文件依然是打开的。
void endhostent(void);
// 函数endhostent可以关闭文件。

struct netent {
  char	*n_name;		/* network name */
  char **n_aliases;		/* alternate network name array pointer */
  int    n_addrtype;	/* address type */
  uint32_t n_net;		/* network number */
  ...
};

struct netent *getnetbyaddr(uint32_t  net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
void setnetent(int stayopen);
void endnetent(void);

struct protoent {
  char	*p_name;	/* protocol name */
  char **p_aliases;	/* pointer to altername protocol name array */
  int	 p_proto;	/* protocol number */
  ...
};

struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
void setprotoent(int stayopen);
void endprotoent(void);

struct servent {
  char	*s_name;	/* service name */
  char **s_aliases;	/* pointer to alternate service name array */
  int	 s_port;	/* port number */
  char  *s_proto;	/* name of protocol */
  ...
};

struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
struct servent *getservent(void);
void setservent(int stayopen);
void endservent(void);

struct addrinfo {
  int			ai_flags;		/* customize behavior */
  int			ai_family;		/* address family */
  int			ai_socktype;	/* socket type */
  int			ai_protocol;	/* protocol */
  socklen_t		ai_addrlen;		/* length in bytes of address */
  struct sockaddr *ai_addr;		/* address */
  char		   *ai_canonname;	/* canonical name of host */
  struct addrinfo *ai_next;		/* next in list */
  ...
};

int getaddrinfo(const char *restrict host, const char *restrict service, const struct addrinfo *restrict hint,
               	struct addrinfo **restrict res);
// 返回值：若成功，返回0；若出错，返回非0错误码
void freeaddrinfo(struct addrinfo *ai);
// 返回值：指向描述错误的字符串的指针
const char *gai_strerror(int error);
int getnameinfo(const struct sockaddr *restrict addr, socklen_t alen,
               	char *restrict host, socklen_t hostlen,
                char *restrict service, socklen_t servlen, int flags);
// 返回值：若成功，返回0；若出错，返回非0值
```
| 标志              | 描述                                            |
| ----------------- | ----------------------------------------------- |
| `NI_DGRAM`        | 服务基于数据报而非基于流                        |
| `NI_NAMEREQD`     | 如果找不到主机名，将其作为一个错误对待          |
| `NI_NOFQDN`       | 对于本地主机，仅返回全限定域名的节点名部分      |
| `NI_NUMERICHOST`  | 返回主机地址的数字格式，而非主机名              |
| `NI_NUMERICSCOPE` | 对于 `IPv6`，返回范围 `ID` 的数字形式，而非名字 |
| `NI_NUMERICSERV`  | 返回服务地址的数字形式（即端口号），而非名字    |

### 2.4 将套接字与地址关联

```cpp
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
// 返回值：若成功，返回0；若出错，返回-1
int getsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
// 返回值：若成功，返回0；若出错，返回-1
int getpeername(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
// 返回值：若成功，返回0；若出错，返回-1
```

- 可以调用 getsockname 函数来发现绑定到套接字上的地址。
- 如果套接字已经和对等方连接，可以调用 getpeername 函数来找到对方的地址。

## 3 建立连接

```cpp
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
// 返回值：若成功，返回0；若出错，返回-1
int listen(int sockfd, int backlog);
// 返回值：若成功，返回0；若出错，返回-1
int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict len);
// 返回值：若成功，返回文件(套接字)描述符；若出错，返回-1
```

- 在 connect 中指定的地址 addr 是我们想要与之通信的服务器地址。如果 sockfd 没有绑定到一个地址，connect 会给调用者绑定一个默认地址。
- connect 函数中 sockfd 非阻塞时，如果连接不能马上建立，返回-1，errno 设置为 EINPROGRESS。
- connect 函数还可以用于无连接的网络服务（SOCK_DGRAM）。这看起来很矛盾，实际上却是不错的选择。如果用  SOCK_DGRAM 套接字调用 connect，传送的报文的目标地址会被设置为 connect 调用中所指定的地址，这样每次传送报文时就不需要再提供地址。另外，仅能接收来自指定地址的报文。
- 服务器调用 listen 函数来宣告它愿意接受连接请求。参数 backlog 提供一个提示，提示系统该进程所要如对的未完成连接请求数量，其实际值由系统决定。一旦队列满，系统就会拒绝多余的连接请求，所以 backlog 的值应该基于服务器期望负载和处理量来选择。
- 一旦服务器调用了 listen，所用的套接字就能接收连接请求。使用 accept 函数获得连接求并建立连接。函数 accept 返回的描述符时套接字描述符，该描述符连接到调用 connect 的客户端。这个新的套接字描述符与监听套接字描述符具有相同的套接字类型和地址族。如果不关心客户端标识，可以将 addr 和 len 设置为NULL。
- 如果没有连接请求在等待，accept 会阻塞直到一个请求到来。如果监听套接字非阻塞，accept 返回 -1，errno 设置为 EAGAIN 或 EWOULDBLOCK。

## 4 数据传输

```cpp
#include <sys/socket.h>
sszie_t send(int sockfd, const void *buf, size_t nbytes, int flags);
// 返回值：若成功，返回发送的字节数；若出错，返回-1
ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags, const struct sockaddr *destaddr, socklen_t destlen);
// 返回值：若成功，返回发送的字节数；若出错，返回-1
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
// 返回值：若成功，返回发送的字节数；若出错，返回-1

struct msghdr {
  void		*msg_name;		/* optional address */
  socklen_t	msg_namelen;	/* address size in bytes */
  struct iovec msg_iov;		/* array of I/O buffers */
  int		msg_iovlen;		/* number of elements in array */
  void		*msg_control;	/* ancillary data */
  socklen_t	msg_controllen;	/* number of ancillary bytes */
  int		msg_flags;		/* flags for received message */
  ...
};
```

```cpp
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
// 返回值：返回数据的字节长度：若无可用数据或对等方已经按序结束，返回0；若出错，返回-1
ssize_t recvfrom(int sockfd, void *restrict buf, size_t len, int flags, 
                 struct sockaddr *restrict addr, socklen_t *restrict addrlen);
// 返回值：返回数据的字节长度：若无可用数据或对等方已经按序结束，返回0；若出错，返回-1
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
// 返回值：返回数据的字节长度：若无可用数据或对等方已经按序结束，返回0；若出错，返回-1
```

| 写入标志        | 描述                                      |
| --------------- | ----------------------------------------- |
| `MSG_CONFIRM`   | 提供链路层反馈以保持地址映射有效          |
| `MSG_DONTROUTE` | 勿将数据报路由出本地网络                  |
| `MSG_DONTWAIT`  | 允许非阻塞操作（等价于使用 `O_NONBLOCK`） |
| `MSG_EOF`       | 发送数据后关闭套接字的发送端              |
| `MSG_EOR`       | 如果协议支持，标记记录结束                |
| `MSG_MORE`      | 延迟发送数据报允许写更多数据              |
| `MSG_NOSIGNAL`  | 在写无连接的套接字时不产生 `SIGPIPE` 信号 |
| `MSG_OOB`       | 如果协议支持，发送带外数据                |

| 读取标志           | 描述                                                 |
| ------------------ | ---------------------------------------------------- |
| `MSG_CMSG_CLOEXEC` | 为 `UNIX` 域套接字上接收的描述符设置执行时关闭标志。 |
| `MSG_DONTWAIT`     | 启用非阻塞操作（等价于使用 `O_NONBLOCK`）            |
| `MSG_ERRQUEUE`     | 接收错误信息作为辅助数据                             |
| `MSG_OOB`          | 如果协议支持，获取带外数据                           |
| `MSG_PEEK`         | 返回数据包内容而不真正取走数据包                     |
| `MSG_TRUNC`        | 即使数据包被截断，也返回数据包的实际长度             |
| `MSG_WAITALL`      | 等待直到所有的数据可用（仅 `SOCK_STREAM`）           |

| `msghdr` 中的 `msg_flags` 标志 | 描述                     |
| ------------------------------ | ------------------------ |
| `MSG_CTRUNC`                   | 控制数据被截断           |
| `MSG_EOR`                      | 接收记录结束符           |
| `MSG_ERRQUEUE`                 | 接收错误信息作为辅助数据 |
| `MSG_OOB`                      | 接收带外数据             |
| `MSG_TRUNC`                    | 一般数据被截断           |

## 5 套接字选项

```cpp
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int option, const void *val, socklen_t len);
// 返回值：若成功，返回0；若出错，返回-1
int getsockopt(int sockfd, int level, int option, void *restrict val, socklen_t *restrict lenp);
// 返回值：若成功，返回0；若出错，返回-1
```

- 参数 level 标识了选项应用的协议。如果选项时通用的套接字层次选项，则 level 设置成 SOL_SOCKET。否则 level 设置成控制这个选项的协议编号。对于 TCP 选项，level 是 IPPROTO_TCP，对于 IP，level 是 IPPROTO_IP。

- 参数 val 根据选项的不同指向一个数据结构或者一个整数。一些选项是 on/off 开关，如果整数非 0，则启用选项。如果整数为 0，则禁止选项。参数 len 指定了 val 指向的对象的大小。

- 参数 option 指定需要设置的设置项。

  | 套接字选项      | 参数 `val` 的类型 | 描述                                                |
  | --------------- | ----------------- | --------------------------------------------------- |
  | `SO_ACCEPTCONN` | `int`             | 返回信息指示该套接字是否能被监听（仅 `getsockopt`） |
  | `SO_BROADDCAST` | `int`             | 如果 `*val` 非 0，广播数据报                        |
  | `SO_DEBUG`      | `int`             | 如果 `*val` 非 0，启用网络驱动调试功能              |
  | `SO_DONTROUTE`  | `int`             | 如果 `*val` 非 0，绕过通常路由                      |
  | `SO_ERROR`      | `int`             | 返回挂起的套接字错误并清除（仅 `getsockopt`）       |
  | `SO_KEEPALIVE`  | `int`             | 如果 `*val` 非 0，启用周期性 `keep-alive` 报文      |
  | `SO_LINGER`     | `struct linger`   | 当还有未发报文而套接字已关闭时，延迟时间            |
  | `SO_OOBINLINE`  | `int`             | 如果 `*val` 非 0，将带外数据放在普通数据中          |
  | `SO_RCVBUF`     | `int`             | 接收缓冲区的字节长度                                |
  | `SO_RCVLOWAT`   | `int`             | 接收调用中返回的最小数据字节数                      |
  | `SO_RCVTIMEO`   | `struct timeval`  | 套接字接收调用的超时值                              |
  | `SO_REUSEADDR`  | `int`             | 如果 `*val` 非 0，重用 `bind` 中的地址              |
  | `SO_SNDBUF`     | `int`             | 发送缓冲区的字节长度                                |
  | `SO_SNDLOWAT`   | `int`             | 发送调用中传送的最小数据字节数                      |
  | `SO_SNDTIMEO`   | `struct timeval`  | 套接字发送调用的超时值                              |
  | `SO_TYPE`       | `int`             | 标识套接字类型（仅 `getsockopt`）                   |

## 6 带外数据

```cpp
#include <sys/socket.h>
int sockatmark(int sockfd);
// 返回值：若在标记处，返回-1；若没有在标记处，返回0；若出错，返回-1
```

- 带外数据是一些通信协议所支持的可选功能，与普通数据相比，它允许更高优先级的数据传输。带外数据先行传输，即使传输队列已经有数据。`TCP` 支持带外数据，但是 `UDP` 不支持。
- `TCP` 将带外数据称为紧急数据。`TCP` 仅支持一个字节的紧急数据，但是允许紧急数据在普通数据传递机制数据流之外传输。为了产生紧急数据，可以在 3 个 `send` 函数中的任何一个里指定 `MSG_OOB` 标志。如果带 `MSG_OOB` 标志发送的字节数超过一个时，最后一个字节将被视为紧急数据字节。
- 如果通过套接字安排了信号的产生，那么紧急数据被接收时，会发送 `SIGURG` 信号。
- 在 `fcntl` 中使用 `F_SETOWN` 命令来设置一个套接字的所有权。
- `TCP` 支持紧急标记的概念，即在普通数据流中紧急数据所在的位置。如果采用套接字选项 `SO_OOBINLINE`，那么可以在普通数据中接收紧急数据。为帮助判断是否已经到达紧急标记，可以使用 `sockatmark`。
- 可以在普通数据流上接收紧急数据，也可以在其中一个 `recv` 函数中采用 `MSG_OOB` 标志在其他队列数据之前接收紧急数据。
- `TCP` 队列仅有一个字节的紧急数据。如果在接收当前数据字节之前又有新的紧急数据到来，那么已有的字节会被丢弃。

## 7 非阻塞和异步 `I/O`

- 非阻塞 `IO`：非阻塞模式下，失败时立即返回，`errno` 设置为 ``EWOULDBLOCK` 或者 `EAGAIN`。
- 异步 `I/O`：在基于套接字的异步 `I/O` 中，当从套接字中读取数据时，或者当套接字写队列中空间变得可用时，可以安排要发送的信号 `SIGIO`。启用异步 `I/O` 是一个两步骤的过程：
  - 建立套接字所有权，这样信号可以被传递到合适的进程。
    - 方法1：在 `fcntl` 中使用 `F_SETOWN` 命令。
    - 方法2：在 `ioctl` 中使用 `FIOSETOWN` 命令。
    - 方法3：在 `ioctl` 中使用 `SIOCSPGRP` 命令。
  - 通知套接字当 `I/O` 操作不会阻塞时发信号。
    - 方法1：在 `fcntl` 中使用 `F_SETFL` 命令并且启用文件标志 `O_ASYNC`。
    - 方法2：在 `ioctl` 中使用 `FIOASYNC` 命令。
