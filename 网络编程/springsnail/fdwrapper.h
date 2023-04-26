#ifndef FDWRAPPER_H
#define FDWRAPPER_H

enum RET_CODE {
  OK = 0,
  NOTHING = 1,
  IOERR = -1,
  CLOSED = -2,
  BUFFER_FULL = -3,
  BUFFER_EMPTY = -4,
  TRY_AGAIN
};

enum OP_TYPE { READ = 0, WRITE, ERROR };

// 设置文件描述符fd为非阻塞IO
int setnonblocking(int fd);

// 监听描述符fd上的可读事件
void add_read_fd(int epollfd, int fd);

// 监听描述符fd上的可写事件
void add_write_fd(int epollfd, int fd);

// 移除对描述符fd的监视
void removefd(int epollfd, int fd);

// 移除对描述符fd的监视，然后关闭描述符fd
void closefd(int epollfd, int fd);

// 修改对描述符fd的监听事件
void modfd(int epollfd, int fd, int ev);

#endif
